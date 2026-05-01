// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGAssetManager.h"
#include "System/RPGLogChannels.h"
#include "GameplayTags/RPGGameplayTags.h"
#include "System/RPGGameData.h"
#include "AbilitySystemGlobals.h"
#include "Character/RPGPawnData.h"
#include "Misc/App.h"
#include "Stats/StatsMisc.h"
#include "Engine/Engine.h"
#include "AbilitySystem/RPGGameplayCueManager.h"
#include "Misc/ScopedSlowTask.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGAssetManager)

const FName FRPGBundles::Equipped("Equipped");

//////////////////////////////////////////////////////////////////////

static FAutoConsoleCommand CVarDumpLoadedAssets(
	TEXT("RPG.DumpLoadedAssets"),
	TEXT("Shows all assets that were loaded via the asset manager and are currently in memory."),
	FConsoleCommandDelegate::CreateStatic(URPGAssetManager::DumpLoadedAssets)
);

//////////////////////////////////////////////////////////////////////

#define STARTUP_JOB_WEIGHTED(JobFunc, JobWeight) StartupJobs.Add(FRPGAssetManagerStartupJob(#JobFunc, [this](const FRPGAssetManagerStartupJob& StartupJob, TSharedPtr<FStreamableHandle>& LoadHandle){JobFunc;}, JobWeight))
#define STARTUP_JOB(JobFunc) STARTUP_JOB_WEIGHTED(JobFunc, 1.f)

//////////////////////////////////////////////////////////////////////

URPGAssetManager::URPGAssetManager()
{
	DefaultPawnData = nullptr;
}

URPGAssetManager& URPGAssetManager::Get()
{
	check(GEngine);

	if (URPGAssetManager* Singleton = Cast<URPGAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogRPG, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to RPGAssetManager!"));

	// Fatal error above prevents this from being called.
	return *NewObject<URPGAssetManager>();
}

UObject* URPGAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsInitialized())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
		}

		// Use LoadObject if asset manager isn't ready yet.
		return AssetPath.TryLoad();
	}

	return nullptr;
}

bool URPGAssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

void URPGAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}

void URPGAssetManager::DumpLoadedAssets()
{
	UE_LOG(LogRPG, Log, TEXT("========== Start Dumping Loaded Assets =========="));

	for (const UObject* LoadedAsset : Get().LoadedAssets)
	{
		UE_LOG(LogRPG, Log, TEXT("  %s"), *GetNameSafe(LoadedAsset));
	}

	UE_LOG(LogRPG, Log, TEXT("... %d assets in loaded pool"), Get().LoadedAssets.Num());
	UE_LOG(LogRPG, Log, TEXT("========== Finish Dumping Loaded Assets =========="));
}

void URPGAssetManager::StartInitialLoading()
{
	SCOPED_BOOT_TIMING("URPGAssetManager::StartInitialLoading");

	// This does all of the scanning, need to do this now even if loads are deferred
	Super::StartInitialLoading();

	STARTUP_JOB(InitializeGameplayCueManager());

	{
		// Load base game data asset
		STARTUP_JOB_WEIGHTED(GetGameData(), 25.f);
	}

	// Run all the queued up startup jobs
	DoAllStartupJobs();
}

void URPGAssetManager::InitializeGameplayCueManager()
{
	SCOPED_BOOT_TIMING("URPGAssetManager::InitializeGameplayCueManager");

	URPGGameplayCueManager* GCM = URPGGameplayCueManager::Get();
	if (!GCM)
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGAssetManager::InitializeGameplayCueManager() - GameplayCueManager not available yet, skipping cue loading."));
		return;
	}
	GCM->LoadAlwaysLoadedCues();
}


const URPGGameData* URPGAssetManager::GetGameData()
{
	return GetOrLoadTypedGameData<URPGGameData>(RPGGameDataPath);
}

const URPGPawnData* URPGAssetManager::GetDefaultPawnData() const
{
	UE_LOG(LogRPG, Log, TEXT("URPGAssetManager::GetDefaultPawnData - Path: %s, IsNull: %s"),
		*DefaultPawnData.ToString(), DefaultPawnData.IsNull() ? TEXT("true") : TEXT("false"));

	const URPGPawnData* Result = GetAsset(DefaultPawnData);

	if (!Result && !DefaultPawnData.IsNull())
	{
		// GameFeature plugin content may not be mounted yet via AssetManager.
		// Try direct synchronous load as a fallback.
		UE_LOG(LogRPG, Log, TEXT("URPGAssetManager::GetDefaultPawnData - AssetManager load failed, trying LoadSynchronous fallback..."));
		Result = DefaultPawnData.LoadSynchronous();
		if (Result)
		{
			UE_LOG(LogRPG, Log, TEXT("URPGAssetManager::GetDefaultPawnData - LoadSynchronous succeeded: %s"), *GetNameSafe(Result));
		}
		else
		{
			UE_LOG(LogRPG, Warning, TEXT("URPGAssetManager::GetDefaultPawnData - Both load methods FAILED! Path: %s. "
				"Check: 1) Asset exists at this path 2) RPGCore GameFeature plugin is mounted"), *DefaultPawnData.ToString());
		}
	}
	else if (Result)
	{
		UE_LOG(LogRPG, Log, TEXT("URPGAssetManager::GetDefaultPawnData - Loaded: %s"), *GetNameSafe(Result));
	}

	return Result;
}

UPrimaryDataAsset* URPGAssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
	UPrimaryDataAsset* Asset = nullptr;

	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Loading GameData Object"), STAT_GameData, STATGROUP_LoadTime);
	if (!DataClassPath.IsNull())
	{
#if WITH_EDITOR
		FScopedSlowTask SlowTask(0, FText::Format(NSLOCTEXT("RPGEditor", "BeginLoadingGameDataTask", "Loading GameData {0}"), FText::FromName(DataClass->GetFName())));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);
#endif
		UE_LOG(LogRPG, Log, TEXT("Loading GameData: %s ..."), *DataClassPath.ToString());
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("    ... GameData loaded!"), nullptr);

		// This can be called recursively in the editor because it is called on demand from PostLoad so force a sync load for primary asset and async load the rest in that case
		if (GIsEditor)
		{
			Asset = DataClassPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(PrimaryAssetType);
		}
		else
		{
			TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
			if (Handle.IsValid())
			{
				Handle->WaitUntilComplete(0.0f, false);

				// This should always work
				Asset = Cast<UPrimaryDataAsset>(Handle->GetLoadedAsset());
			}
		}
	}

	if (Asset)
	{
		GameDataMap.Add(DataClass, Asset);
	}
	else
	{
		// P0: BP_RPGGameData 尚未创建前，容错处理避免 Fatal
		UE_LOG(LogRPG, Warning, TEXT("Failed to load GameData asset at %s. Type %s. This is expected if BP_RPGGameData has not been created yet (T12)."), *DataClassPath.ToString(), *PrimaryAssetType.ToString());
	}

	return Asset;
}


void URPGAssetManager::DoAllStartupJobs()
{
	SCOPED_BOOT_TIMING("URPGAssetManager::DoAllStartupJobs");
	const double AllStartupJobsStartTime = FPlatformTime::Seconds();

	if (IsRunningDedicatedServer())
	{
		// No need for periodic progress updates, just run the jobs
		for (const FRPGAssetManagerStartupJob& StartupJob : StartupJobs)
		{
			StartupJob.DoJob();
		}
	}
	else
	{
		if (StartupJobs.Num() > 0)
		{
			float TotalJobValue = 0.0f;
			for (const FRPGAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				TotalJobValue += StartupJob.JobWeight;
			}

			float AccumulatedJobValue = 0.0f;
			for (FRPGAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				const float JobValue = StartupJob.JobWeight;
				StartupJob.SubstepProgressDelegate.BindLambda([This = this, AccumulatedJobValue, JobValue, TotalJobValue](float NewProgress)
					{
						const float SubstepAdjustment = FMath::Clamp(NewProgress, 0.0f, 1.0f) * JobValue;
						const float OverallPercentWithSubstep = (AccumulatedJobValue + SubstepAdjustment) / TotalJobValue;

						This->UpdateInitialGameContentLoadPercent(OverallPercentWithSubstep);
					});

				StartupJob.DoJob();

				StartupJob.SubstepProgressDelegate.Unbind();

				AccumulatedJobValue += JobValue;

				UpdateInitialGameContentLoadPercent(AccumulatedJobValue / TotalJobValue);
			}
		}
		else
		{
			UpdateInitialGameContentLoadPercent(1.0f);
		}
	}

	StartupJobs.Empty();

	UE_LOG(LogRPG, Display, TEXT("All startup jobs took %.2f seconds to complete"), FPlatformTime::Seconds() - AllStartupJobsStartTime);
}

void URPGAssetManager::UpdateInitialGameContentLoadPercent(float GameContentPercent)
{
	// Could route this to the early startup loading screen
}

#if WITH_EDITOR
void URPGAssetManager::PreBeginPIE(bool bStartSimulate)
{
	Super::PreBeginPIE(bStartSimulate);

	{
		FScopedSlowTask SlowTask(0, NSLOCTEXT("RPGEditor", "BeginLoadingPIEData", "Loading PIE Data"));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);

		const URPGGameData* LocalGameDataCommon = GetGameData();

		if (!LocalGameDataCommon)
		{
			UE_LOG(LogRPG, Warning, TEXT("PreBeginPIE: Failed to load RPGGameData, skipping PIE preloading."));
			return;
		}

		// Intentionally after GetGameData to avoid counting GameData time in this timer
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("PreBeginPIE asset preloading complete"), nullptr);

		// You could add preloading of anything else needed for the experience we'll be using here
		// (e.g., by grabbing the default experience from the world settings + the experience override in developer settings)
	}
}
#endif
