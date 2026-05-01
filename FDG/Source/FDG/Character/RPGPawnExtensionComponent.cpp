// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGPawnExtensionComponent.h"
#include "Character/RPGPawnData.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/RPGAbilitySet.h"
#include "GameplayTags/RPGGameplayTags.h"
#include "Player/RPGHeroComponent.h"
#include "Input/RPGInputConfig.h"
#include "System/RPGLogChannels.h"
#include "System/RPGAssetManager.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerState.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGPawnExtensionComponent)

URPGPawnExtensionComponent::URPGPawnExtensionComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = true;

	InitState = RPGGameplayTags::InitState_Spawned;
}

void URPGPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	// Start with Spawned state
	InitState = RPGGameplayTags::InitState_Spawned;
}

void URPGPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Start ticking to drive the init state machine
	SetComponentTickEnabled(true);
}

void URPGPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Uninitialize the ability system
	OnAbilitySystemUninitialized();

	Super::EndPlay(EndPlayReason);
}

void URPGPawnExtensionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Drive the initialization state machine
	if (UpdateInitState(DeltaTime))
	{
		// State changed, we can stop ticking if we're ready
		if (InitState == RPGGameplayTags::InitState_GameplayReady)
		{
			SetComponentTickEnabled(false);
		}
	}
}

URPGPawnExtensionComponent* URPGPawnExtensionComponent::FindPawnExtensionComponent(const AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	return Actor->FindComponentByClass<URPGPawnExtensionComponent>();
}

void URPGPawnExtensionComponent::SetPawnData(const URPGPawnData* InPawnData)
{
	check(InPawnData);

	if (PawnData)
	{
		UE_LOG(LogRPG, Error, TEXT("URPGPawnExtensionComponent::SetPawnData - PawnData already set for %s"), *GetNameSafe(GetOwner()));
		return;
	}

	PawnData = InPawnData;

	UE_LOG(LogRPG, Log, TEXT("URPGPawnExtensionComponent::SetPawnData - Set PawnData '%s' for %s, Current InitState: %s"),
		*GetNameSafe(InPawnData), *GetNameSafe(GetOwner()), *InitState.ToString());

	// 诊断日志：打印 PawnData 各属性，方便排查空引用
	UE_LOG(LogRPG, Log, TEXT("  PawnData '%s' Properties:"), *GetNameSafe(InPawnData));
	UE_LOG(LogRPG, Log, TEXT("    PawnClass: %s"), InPawnData->PawnClass.IsNull() ? TEXT("NULL") : *InPawnData->PawnClass.ToString());
	UE_LOG(LogRPG, Log, TEXT("    InputConfig: %s"), InPawnData->InputConfig.IsNull() ? TEXT("NULL <<<< MISSING!") : *InPawnData->InputConfig.ToString());
	UE_LOG(LogRPG, Log, TEXT("    InputMappingContext: %s"), InPawnData->InputMappingContext.IsNull() ? TEXT("NULL <<<< MISSING!") : *InPawnData->InputMappingContext.ToString());
	UE_LOG(LogRPG, Log, TEXT("    DefaultCameraMode: %s"), InPawnData->DefaultCameraMode.IsNull() ? TEXT("NULL <<<< MISSING!") : *InPawnData->DefaultCameraMode.ToString());
	UE_LOG(LogRPG, Log, TEXT("    AbilitySets: %d entries"), InPawnData->AbilitySets.Num());
	UE_LOG(LogRPG, Log, TEXT("    TagRelationshipMapping: %s"), InPawnData->TagRelationshipMapping ? TEXT("Set") : TEXT("NULL"));

	// Notify that pawn data has changed
	OnPawnDataChanged.Broadcast();

	// Force an init state update by enabling tick
	SetComponentTickEnabled(true);
}

bool URPGPawnExtensionComponent::CanChangeInitState(FGameplayTag DesiredState) const
{
	// State machine transitions:
	// Spawned -> DataAvailable: PawnData must be set
	// DataAvailable -> DataInitialized: All dependent data must be ready
	// DataInitialized -> GameplayReady: Everything is ready

	if (DesiredState == RPGGameplayTags::InitState_DataAvailable)
	{
		// PawnData must be set
		if (!PawnData)
		{
			return false;
		}
	}
	else if (DesiredState == RPGGameplayTags::InitState_DataInitialized)
	{
		// For player-controlled pawns, we need a controller
		APawn* Pawn = Cast<APawn>(GetOwner());
		if (!Pawn)
		{
			UE_LOG(LogRPG, Warning, TEXT("URPGPawnExtensionComponent::CanChangeInitState - Owner is not a Pawn!"));
			return false;
		}

		// Player-controlled pawns need a controller to be fully initialized
		if (Pawn->GetController() == nullptr)
		{
			// AI pawns don't need a player controller
			if (Pawn->GetOwner() == nullptr)
			{
				UE_LOG(LogRPG, Verbose, TEXT("URPGPawnExtensionComponent::CanChangeInitState - Pawn '%s' has no Controller and no Owner, waiting"), *GetNameSafe(Pawn));
				return false;
			}
		}
	}
	else if (DesiredState == RPGGameplayTags::InitState_GameplayReady)
	{
		// Must be at DataInitialized to transition to GameplayReady
		if (InitState != RPGGameplayTags::InitState_DataInitialized)
		{
			return false;
		}
	}

	return true;
}

bool URPGPawnExtensionComponent::UpdateInitState(float DeltaTime)
{
	// Check if we can transition to the next state
	FGameplayTag NextState = InitState;

	if (NextState == RPGGameplayTags::InitState_Spawned)
	{
		if (CanChangeInitState(RPGGameplayTags::InitState_DataAvailable))
		{
			NextState = RPGGameplayTags::InitState_DataAvailable;
		}
	}

	if (NextState == RPGGameplayTags::InitState_DataAvailable)
	{
		if (CanChangeInitState(RPGGameplayTags::InitState_DataInitialized))
		{
			NextState = RPGGameplayTags::InitState_DataInitialized;
		}
	}

	if (NextState == RPGGameplayTags::InitState_DataInitialized)
	{
		if (CanChangeInitState(RPGGameplayTags::InitState_GameplayReady))
		{
			NextState = RPGGameplayTags::InitState_GameplayReady;
		}
	}

	// If state changed, handle the transition
	if (NextState != InitState)
	{
		FGameplayTag OldState = InitState;
		InitState = NextState;
		HandleInitStateChanged(OldState, NextState);
		return true;
	}

	return false;
}

void URPGPawnExtensionComponent::HandleInitStateChanged(FGameplayTag OldState, FGameplayTag NewState)
{
	UE_LOG(LogRPG, Log, TEXT("URPGPawnExtensionComponent::HandleInitStateChanged - %s: %s -> %s"),
		*GetNameSafe(GetOwner()), *OldState.ToString(), *NewState.ToString());

	if (NewState == RPGGameplayTags::InitState_DataAvailable)
	{
		// PawnData is available, nothing special to do here
		UE_LOG(LogRPG, Log, TEXT("  -> PawnData '%s' is available"), *GetNameSafe(PawnData));
	}
	else if (NewState == RPGGameplayTags::InitState_DataInitialized)
	{
		// Data is initialized, broadcast ready to initialize
		if (!bPawnReadyToInitializeBroadcast)
		{
			bPawnReadyToInitializeBroadcast = true;
			OnPawnReadyToInitialize.Broadcast();

			// Initialize the ability system
			OnAbilitySystemInitialized();

			// Set up player input (if this is a player-controlled pawn)
			SetupPlayerInputComponent();
		}
	}
	else if (NewState == RPGGameplayTags::InitState_GameplayReady)
	{
		UE_LOG(LogRPG, Log, TEXT("  -> Pawn '%s' is GameplayReady"), *GetNameSafe(GetOwner()));
	}
}

bool URPGPawnExtensionComponent::IsReadyToInitialize() const
{
	return InitState == RPGGameplayTags::InitState_DataInitialized ||
		InitState == RPGGameplayTags::InitState_GameplayReady;
}

void URPGPawnExtensionComponent::HandleControllerChanged()
{
	// If we're not yet initialized, try to advance the state machine
	if (InitState != RPGGameplayTags::InitState_GameplayReady)
	{
		SetComponentTickEnabled(true);
	}
}

void URPGPawnExtensionComponent::OnPlayerStateChanged()
{
	// If we're not yet initialized, try to advance the state machine
	if (InitState != RPGGameplayTags::InitState_GameplayReady)
	{
		SetComponentTickEnabled(true);
	}
}

void URPGPawnExtensionComponent::SetupPlayerInputComponent()
{
	// Delegate input setup to the HeroComponent on the PlayerController
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGPawnExtensionComponent::SetupPlayerInputComponent - Owner is not a Pawn"));
		return;
	}

	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (!PC)
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGPawnExtensionComponent::SetupPlayerInputComponent - Pawn '%s' has no PlayerController"), *GetNameSafe(Pawn));
		return;
	}

	URPGHeroComponent* HeroComp = PC->FindComponentByClass<URPGHeroComponent>();
	if (!HeroComp)
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGPawnExtensionComponent::SetupPlayerInputComponent - PC '%s' has no HeroComponent"), *GetNameSafe(PC));
		return;
	}

	// Get the InputConfig from PawnData
	if (!PawnData)
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGPawnExtensionComponent::SetupPlayerInputComponent - PawnData is null"));
		return;
	}

	if (const URPGInputConfig* InputConfig = PawnData->InputConfig.Get())
	{
		UE_LOG(LogRPG, Log, TEXT("URPGPawnExtensionComponent::SetupPlayerInputComponent - Initializing input with config '%s'"), *GetNameSafe(InputConfig));
		HeroComp->InitializePlayerInput(InputConfig);
	}
	else if (!PawnData->InputConfig.IsNull())
	{
		UE_LOG(LogRPG, Log, TEXT("URPGPawnExtensionComponent::SetupPlayerInputComponent - Async loading InputConfig '%s'"), *PawnData->InputConfig.ToString());
		// Load async and bind when ready
		TSoftObjectPtr<URPGInputConfig> InputConfigPtr = PawnData->InputConfig;
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		StreamableManager.RequestAsyncLoad(InputConfigPtr.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this, [this, HeroComp, InputConfigPtr]()
		{
			if (const URPGInputConfig* LoadedConfig = InputConfigPtr.Get())
			{
				HeroComp->InitializePlayerInput(LoadedConfig);
			}
		}));
	}
	else
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGPawnExtensionComponent::SetupPlayerInputComponent - PawnData '%s' has no InputConfig set!"), *GetNameSafe(PawnData));
	}
}

bool URPGPawnExtensionComponent::IsLocalPlayerController() const
{
	if (const APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (const AController* Controller = Pawn->GetController())
		{
			return Controller->IsLocalPlayerController();
		}
	}
	return false;
}

URPGAbilitySystemComponent* URPGPawnExtensionComponent::GetAbilitySystemComponent() const
{
	if (const APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (const APlayerState* PS = Pawn->GetPlayerState())
		{
			return PS->FindComponentByClass<URPGAbilitySystemComponent>();
		}
	}
	return nullptr;
}

void URPGPawnExtensionComponent::OnAbilitySystemInitialized()
{
	if (bAbilitySystemInitialized)
	{
		return;
	}

	if (URPGAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		bAbilitySystemInitialized = true;

		// Grant ability sets from PawnData
		if (PawnData)
		{
			UE_LOG(LogRPG, Log, TEXT("URPGPawnExtensionComponent::OnAbilitySystemInitialized - Granting %d AbilitySets for %s"),
				PawnData->AbilitySets.Num(), *GetNameSafe(GetOwner()));

			for (int32 i = 0; i < PawnData->AbilitySets.Num(); ++i)
			{
				const TSoftObjectPtr<URPGAbilitySet>& AbilitySetPtr = PawnData->AbilitySets[i];

				const URPGAbilitySet* AbilitySet = AbilitySetPtr.Get();
				if (!AbilitySet && !AbilitySetPtr.IsNull())
				{
					// Soft pointer not loaded yet (common with GameFeature plugins), force load
					AbilitySet = AbilitySetPtr.LoadSynchronous();
					UE_LOG(LogRPG, Log, TEXT("  AbilitySet[%d] - LoadSynchronous fallback: %s"), i, *GetNameSafe(AbilitySet));
				}

				if (AbilitySet)
				{
					UE_LOG(LogRPG, Log, TEXT("  AbilitySet[%d] - Granting '%s' with %d abilities"), i, *GetNameSafe(AbilitySet), AbilitySet->Abilities.Num());
					AbilitySet->GiveToAbilitySystem(ASC);
				}
				else
				{
					UE_LOG(LogRPG, Warning, TEXT("  AbilitySet[%d] - FAILED to load! Path: %s"), i, *AbilitySetPtr.ToString());
				}
			}
		}

		UE_LOG(LogRPG, Log, TEXT("URPGPawnExtensionComponent::OnAbilitySystemInitialized - ASC initialized, ActivatableAbilities: %d"),
			ASC->GetActivatableAbilities().Num());
	}
}

void URPGPawnExtensionComponent::OnAbilitySystemUninitialized()
{
	if (!bAbilitySystemInitialized)
	{
		return;
	}

	bAbilitySystemInitialized = false;
	bPawnReadyToInitializeBroadcast = false;

	// Reset init state
	InitState = RPGGameplayTags::InitState_Spawned;
}
