// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGCheatManager.h"
#include "GameFramework/Pawn.h"
#include "RPGPlayerController.h"
#include "RPGDebugCameraController.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Console.h"
#include "GameFramework/HUD.h"
#include "System/RPGAssetManager.h"
#include "System/RPGGameData.h"
#include "RPGGameplayTags.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Character/RPGHealthComponent.h"
#include "Character/RPGPawnExtensionComponent.h"
#include "System/RPGSystemStatics.h"
#include "Development/RPGDeveloperSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGCheatManager)

DEFINE_LOG_CATEGORY(LogRPGCheat);

namespace RPGCheat
{
	static const FName NAME_Fixed = FName(TEXT("Fixed"));
	
	static bool bEnableDebugCameraCycling = false;
	static FAutoConsoleVariableRef CVarEnableDebugCameraCycling(
		TEXT("LyraCheat.EnableDebugCameraCycling"),
		bEnableDebugCameraCycling,
		TEXT("If true then you can cycle the debug camera while running the game."),
		ECVF_Cheat);

	static bool bStartInGodMode = false;
	static FAutoConsoleVariableRef CVarStartInGodMode(
		TEXT("LyraCheat.StartInGodMode"),
		bStartInGodMode,
		TEXT("If true then the God cheat will be applied on begin play"),
		ECVF_Cheat);
};


URPGCheatManager::URPGCheatManager()
{
	DebugCameraControllerClass = ARPGDebugCameraController::StaticClass();
}

void URPGCheatManager::InitCheatManager()
{
	Super::InitCheatManager();

#if WITH_EDITOR
	if (GIsEditor)
	{
		APlayerController* PC = GetOuterAPlayerController();
		for (const FRPGCheatToRun& CheatRow : GetDefault<URPGDeveloperSettings>()->CheatsToRun)
		{
			if (CheatRow.Phase == ECheatExecutionTime::OnCheatManagerCreated)
			{
				PC->ConsoleCommand(CheatRow.Cheat, /*bWriteToLog=*/ true);
			}
		}
	}
#endif

	if (RPGCheat::bStartInGodMode)
	{
		God();	
	}
}

void URPGCheatManager::CheatOutputText(const FString& TextToOutput)
{
#if USING_CHEAT_MANAGER
	// Output to the console.
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->ViewportConsole)
	{
		GEngine->GameViewport->ViewportConsole->OutputText(TextToOutput);
	}

	// Output to log.
	UE_LOG(LogRPGCheat, Display, TEXT("%s"), *TextToOutput);
#endif // USING_CHEAT_MANAGER
}

void URPGCheatManager::Cheat(const FString& Msg)
{
	if (ARPGPlayerController* RPGPC = Cast<ARPGPlayerController>(GetOuterAPlayerController()))
	{
		RPGPC->ServerCheat(Msg.Left(128));
	}
}

void URPGCheatManager::CheatAll(const FString& Msg)
{
	if (ARPGPlayerController* RPGPC = Cast<ARPGPlayerController>(GetOuterAPlayerController()))
	{
		RPGPC->ServerCheatAll(Msg.Left(128));
	}
}

void URPGCheatManager::PlayNextGame()
{
	URPGSystemStatics::PlayNextGame(this);
}

void URPGCheatManager::EnableDebugCamera()
{
	Super::EnableDebugCamera();
}

void URPGCheatManager::DisableDebugCamera()
{
	FVector DebugCameraLocation;
	FRotator DebugCameraRotation;

	ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* OriginalPC = nullptr;

	if (DebugCC)
	{
		OriginalPC = DebugCC->OriginalControllerRef;
		DebugCC->GetPlayerViewPoint(DebugCameraLocation, DebugCameraRotation);
	}

	Super::DisableDebugCamera();

	if (OriginalPC && OriginalPC->PlayerCameraManager && (OriginalPC->PlayerCameraManager->CameraStyle == RPGCheat::NAME_Fixed))
	{
		OriginalPC->SetInitialLocationAndRotation(DebugCameraLocation, DebugCameraRotation);

		OriginalPC->PlayerCameraManager->ViewTarget.POV.Location = DebugCameraLocation;
		OriginalPC->PlayerCameraManager->ViewTarget.POV.Rotation = DebugCameraRotation;
		OriginalPC->PlayerCameraManager->PendingViewTarget.POV.Location = DebugCameraLocation;
		OriginalPC->PlayerCameraManager->PendingViewTarget.POV.Rotation = DebugCameraRotation;
	}
}

bool URPGCheatManager::InDebugCamera() const
{
	return (Cast<ADebugCameraController>(GetOuter()) ? true : false);
}

void URPGCheatManager::EnableFixedCamera()
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		PC->SetCameraMode(RPGCheat::NAME_Fixed);
	}
}

void URPGCheatManager::DisableFixedCamera()
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		PC->SetCameraMode(NAME_Default);
	}
}

bool URPGCheatManager::InFixedCamera() const
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	const APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		return (PC->PlayerCameraManager->CameraStyle == RPGCheat::NAME_Fixed);
	}

	return false;
}

void URPGCheatManager::ToggleFixedCamera()
{
	if (InFixedCamera())
	{
		DisableFixedCamera();
	}
	else
	{
		EnableFixedCamera();
	}
}

void URPGCheatManager::CycleDebugCameras()
{
	if (!RPGCheat::bEnableDebugCameraCycling)
	{
		return;
	}
	
	if (InDebugCamera())
	{
		EnableFixedCamera();
		DisableDebugCamera();
	}
	else if (InFixedCamera())
	{
		DisableFixedCamera();
		DisableDebugCamera();
	}
	else
	{
		EnableDebugCamera();
		DisableFixedCamera();
	}
}

void URPGCheatManager::CycleAbilitySystemDebug()
{
	APlayerController* PC = Cast<APlayerController>(GetOuterAPlayerController());

	if (PC && PC->MyHUD)
	{
		if (!PC->MyHUD->bShowDebugInfo || !PC->MyHUD->DebugDisplay.Contains(TEXT("AbilitySystem")))
		{
			PC->MyHUD->ShowDebug(TEXT("AbilitySystem"));
		}

		PC->ConsoleCommand(TEXT("AbilitySystem.Debug.NextCategory"));
	}
}

void URPGCheatManager::CancelActivatedAbilities()
{
	if (URPGAbilitySystemComponent* RPGASC = GetPlayerAbilitySystemComponent())
	{
		const bool bReplicateCancelAbility = true;
		RPGASC->CancelInputActivatedAbilities(bReplicateCancelAbility);
	}
}

void URPGCheatManager::AddTagToSelf(FString TagName)
{
	FGameplayTag Tag = RPGGameplayTags::FindTagByString(TagName, true);
	if (Tag.IsValid())
	{
		if (URPGAbilitySystemComponent* RPGASC = GetPlayerAbilitySystemComponent())
		{
			RPGASC->AddDynamicTagGameplayEffect(Tag);
		}
	}
	else
	{
		UE_LOG(LogRPGCheat, Display, TEXT("AddTagToSelf: Could not find any tag matching [%s]."), *TagName);
	}
}

void URPGCheatManager::RemoveTagFromSelf(FString TagName)
{
	FGameplayTag Tag = RPGGameplayTags::FindTagByString(TagName, true);
	if (Tag.IsValid())
	{
		if (URPGAbilitySystemComponent* RPGASC = GetPlayerAbilitySystemComponent())
		{
			RPGASC->RemoveDynamicTagGameplayEffect(Tag);
		}
	}
	else
	{
		UE_LOG(LogRPGCheat, Display, TEXT("RemoveTagFromSelf: Could not find any tag matching [%s]."), *TagName);
	}
}

void URPGCheatManager::DamageSelf(float DamageAmount)
{
	if (URPGAbilitySystemComponent* RPGASC = GetPlayerAbilitySystemComponent())
	{
		ApplySetByCallerDamage(RPGASC, DamageAmount);
	}
}

void URPGCheatManager::DamageTarget(float DamageAmount)
{
	if (ARPGPlayerController* RPGPC = Cast<ARPGPlayerController>(GetOuterAPlayerController()))
	{
		if (RPGPC->GetNetMode() == NM_Client)
		{
			// Automatically send cheat to server for convenience.
			RPGPC->ServerCheat(FString::Printf(TEXT("DamageTarget %.2f"), DamageAmount));
			return;
		}

		FHitResult TargetHitResult;
		AActor* TargetActor = GetTarget(RPGPC, TargetHitResult);

		if (URPGAbilitySystemComponent* RPGTargetASC = Cast<URPGAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor)))
		{
			ApplySetByCallerDamage(RPGTargetASC, DamageAmount);
		}
	}
}

void URPGCheatManager::ApplySetByCallerDamage(URPGAbilitySystemComponent* RPGASC, float DamageAmount)
{
	check(RPGASC);

	TSubclassOf<UGameplayEffect> DamageGE = URPGAssetManager::GetSubclass(URPGGameData::Get().DamageGameplayEffect_SetByCaller);
	FGameplayEffectSpecHandle SpecHandle = RPGASC->MakeOutgoingSpec(DamageGE, 1.0f, RPGASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(RPGGameplayTags::SetByCaller_Damage, DamageAmount);
		RPGASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void URPGCheatManager::HealSelf(float HealAmount)
{
	if (URPGAbilitySystemComponent* RPGASC = GetPlayerAbilitySystemComponent())
	{
		ApplySetByCallerHeal(RPGASC, HealAmount);
	}
}

void URPGCheatManager::HealTarget(float HealAmount)
{
	if (ARPGPlayerController* RPGPC = Cast<ARPGPlayerController>(GetOuterAPlayerController()))
	{
		FHitResult TargetHitResult;
		AActor* TargetActor = GetTarget(RPGPC, TargetHitResult);

		if (URPGAbilitySystemComponent* RPGTargetASC = Cast<URPGAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor)))
		{
			ApplySetByCallerHeal(RPGTargetASC, HealAmount);
		}
	}
}

void URPGCheatManager::ApplySetByCallerHeal(URPGAbilitySystemComponent* RPGASC, float HealAmount)
{
	check(RPGASC);

	TSubclassOf<UGameplayEffect> HealGE = URPGAssetManager::GetSubclass(URPGGameData::Get().HealGameplayEffect_SetByCaller);
	FGameplayEffectSpecHandle SpecHandle = RPGASC->MakeOutgoingSpec(HealGE, 1.0f, RPGASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(RPGGameplayTags::SetByCaller_Heal, HealAmount);
		RPGASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

URPGAbilitySystemComponent* URPGCheatManager::GetPlayerAbilitySystemComponent() const
{
	if (ARPGPlayerController* RPGPC = Cast<ARPGPlayerController>(GetOuterAPlayerController()))
	{
		return RPGPC->GetRPGAbilitySystemComponent();
	}
	return nullptr;
}

void URPGCheatManager::DamageSelfDestruct()
{
	if (ARPGPlayerController* RPGPC = Cast<ARPGPlayerController>(GetOuterAPlayerController()))
	{
 		if (const URPGPawnExtensionComponent* PawnExtComp = URPGPawnExtensionComponent::FindPawnExtensionComponent(RPGPC->GetPawn()))
		{
			if (PawnExtComp->HasReachedInitState(RPGGameplayTags::InitState_GameplayReady))
			{
				if (URPGHealthComponent* HealthComponent = URPGHealthComponent::FindHealthComponent(RPGPC->GetPawn()))
				{
					HealthComponent->DamageSelfDestruct();
				}
			}
		}
	}
}

void URPGCheatManager::God()
{
	if (ARPGPlayerController* RPGPC = Cast<ARPGPlayerController>(GetOuterAPlayerController()))
	{
		if (RPGPC->GetNetMode() == NM_Client)
		{
			// Automatically send cheat to server for convenience.
			RPGPC->ServerCheat(FString::Printf(TEXT("God")));
			return;
		}

		if (URPGAbilitySystemComponent* RPGASC = RPGPC->GetRPGAbilitySystemComponent())
		{
			const FGameplayTag Tag = RPGGameplayTags::Cheat_GodMode;
			const bool bHasTag = RPGASC->HasMatchingGameplayTag(Tag);

			if (bHasTag)
			{
				RPGASC->RemoveDynamicTagGameplayEffect(Tag);
			}
			else
			{
				RPGASC->AddDynamicTagGameplayEffect(Tag);
			}
		}
	}
}

void URPGCheatManager::UnlimitedHealth(int32 Enabled)
{
	if (URPGAbilitySystemComponent* RPGASC = GetPlayerAbilitySystemComponent())
	{
		const FGameplayTag Tag = RPGGameplayTags::Cheat_UnlimitedHealth;
		const bool bHasTag = RPGASC->HasMatchingGameplayTag(Tag);

		if ((Enabled == -1) || ((Enabled > 0) && !bHasTag) || ((Enabled == 0) && bHasTag))
		{
			if (bHasTag)
			{
				RPGASC->RemoveDynamicTagGameplayEffect(Tag);
			}
			else
			{
				RPGASC->AddDynamicTagGameplayEffect(Tag);
			}
		}
	}
}

