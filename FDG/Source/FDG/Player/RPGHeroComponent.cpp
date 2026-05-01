// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGHeroComponent.h"
#include "Input/RPGInputConfig.h"
#include "Input/RPGInputComponent.h"
#include "Character/RPGPawnData.h"
#include "Character/RPGPawnExtensionComponent.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Player/RPGPlayerController.h"
#include "Player/RPGPlayerState.h"
#include "Character/RPGCharacter.h"
#include "GameplayTags/RPGGameplayTags.h"
#include "System/RPGLogChannels.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGHeroComponent)

URPGHeroComponent::URPGHeroComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void URPGHeroComponent::BeginPlay()
{
	Super::BeginPlay();
}

void URPGHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 清理 IMC 映射
	if (CurrentInputMappingContext)
	{
		RemoveInputMappingContext(CurrentInputMappingContext);
		CurrentInputMappingContext = nullptr;
	}

	// 清理能力输入绑定
	if (URPGInputComponent* RPGInputComp = Cast<URPGInputComponent>(GetOwner()->FindComponentByClass<UInputComponent>()))
	{
		RPGInputComp->RemoveBinds(AbilityInputBindHandles);
	}

	Super::EndPlay(EndPlayReason);
}

void URPGHeroComponent::InitializePlayerInput(const URPGInputConfig* InputConfig)
{
	if (bInputInitialized)
	{
		return;
	}

	if (!InputConfig)
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGHeroComponent::InitializePlayerInput - InputConfig is null"));
		return;
	}

	// Get the player controller
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGHeroComponent::InitializePlayerInput - Owner is not a PlayerController"));
		return;
	}

	// Get the pawn
	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGHeroComponent::InitializePlayerInput - No pawn possessed"));
		return;
	}

	bReadyToBindInput = true;

	// Push the InputMappingContext from PawnData
	if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			// Try to get the IMC from PawnData via PawnExtensionComponent
			if (const URPGPawnExtensionComponent* PawnExt = Pawn->FindComponentByClass<URPGPawnExtensionComponent>())
			{
				if (const URPGPawnData* PawnData = PawnExt->GetPawnData())
				{
					if (!PawnData->InputMappingContext.IsNull())
					{
						if (UInputMappingContext* IMC = PawnData->InputMappingContext.LoadSynchronous())
						{
							Subsystem->AddMappingContext(IMC, 0);
							CurrentInputMappingContext = IMC;
							CurrentIMCPriority = 0;

							UE_LOG(LogRPG, Log, TEXT("URPGHeroComponent::InitializePlayerInput - Pushed IMC: %s"), *GetNameSafe(IMC));
						}
						else
						{
							UE_LOG(LogRPG, Warning, TEXT("URPGHeroComponent::InitializePlayerInput - Failed to load IMC: %s"), *PawnData->InputMappingContext.ToString());
						}
					}
					else
					{
						UE_LOG(LogRPG, Warning, TEXT("URPGHeroComponent::InitializePlayerInput - PawnData has no InputMappingContext set"));
					}
				}
			}
		}
	}

	// Set up native input bindings
	SetupNativeInputBindings(InputConfig);

	// Set up ability input bindings
	SetupAbilityInputBindings(InputConfig);

	bInputInitialized = true;

	UE_LOG(LogRPG, Log, TEXT("URPGHeroComponent::InitializePlayerInput - Input initialized successfully"));
}

void URPGHeroComponent::SetupNativeInputBindings(const URPGInputConfig* InputConfig)
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC || !PC->GetPawn())
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGHeroComponent::SetupNativeInputBindings - No PC or Pawn"));
		return;
	}

	// Get the input component from the PlayerController (where we created URPGInputComponent)
	URPGInputComponent* RPGInputComp = Cast<URPGInputComponent>(PC->InputComponent);
	if (!RPGInputComp)
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGHeroComponent::SetupNativeInputBindings - PC->InputComponent is not URPGInputComponent (actual: %s)"),
			*GetNameSafe(PC->InputComponent));
		return;
	}

	// Get the character for native input callbacks
	ARPGCharacter* RPGCharacter = Cast<ARPGCharacter>(PC->GetPawn());
	if (!RPGCharacter)
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGHeroComponent::SetupNativeInputBindings - Pawn is not an ARPGCharacter (actual: %s)"),
			*GetNameSafe(PC->GetPawn()));
		return;
	}

	// Bind Move input (IA_Move -> InputTag.Move)
	if (const UInputAction* MoveAction = InputConfig->FindNativeInputActionForTag(RPGGameplayTags::InputTag_Move))
	{
		RPGInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, RPGCharacter, &ARPGCharacter::Move);
		UE_LOG(LogRPG, Log, TEXT("URPGHeroComponent::SetupNativeInputBindings - Bound IA_Move"));
	}
	else
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGHeroComponent::SetupNativeInputBindings - No native input action found for InputTag_Move! "
			"Make sure BP_RPGInputConfig has an entry with bIsNative=true and InputTag=InputTag.Move"));
	}

	// IA_MouseMove 不绑定 C++ 回调 — 俯视角游戏相机不随鼠标移动旋转，
	// 角色朝向由 bOrientRotationToMovement 决定。
	// IA_MouseMove 仅在 IMC 中用于映射鼠标移动事件（如未来光标悬停检测等）。
}

void URPGHeroComponent::SetupAbilityInputBindings(const URPGInputConfig* InputConfig)
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC || !PC->GetPawn())
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGHeroComponent::SetupAbilityInputBindings - No PC or Pawn"));
		return;
	}

	// Get the input component from the PlayerController
	URPGInputComponent* RPGInputComp = Cast<URPGInputComponent>(PC->InputComponent);
	if (!RPGInputComp)
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGHeroComponent::SetupAbilityInputBindings - No RPGInputComponent on PC (actual: %s)"),
			*GetNameSafe(PC->InputComponent));
		return;
	}

	// Get the ASC from the player state
	ARPGPlayerState* RPGPS = PC->GetPlayerState<ARPGPlayerState>();
	if (!RPGPS)
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGHeroComponent::SetupAbilityInputBindings - No RPGPlayerState"));
		return;
	}

	URPGAbilitySystemComponent* ASC = RPGPS->GetRPGAbilitySystemComponent();
	if (!ASC)
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGHeroComponent::SetupAbilityInputBindings - No ASC on PlayerState '%s'"), *GetNameSafe(RPGPS));
		return;
	}

	UE_LOG(LogRPG, Log, TEXT("URPGHeroComponent::SetupAbilityInputBindings - InputConfig '%s' has %d entries, ASC has %d activatable abilities"),
		*GetNameSafe(InputConfig), InputConfig->InputActions.Num(), ASC->GetActivatableAbilities().Num());

	// Bind ability actions (pressed -> ASC->AbilityInputPressed, released -> ASC->AbilityInputReleased)
	RPGInputComp->BindAbilityActions(InputConfig, ASC, AbilityInputBindHandles);

	UE_LOG(LogRPG, Log, TEXT("URPGHeroComponent::SetupAbilityInputBindings - Bound ability actions from InputConfig '%s'"), *GetNameSafe(InputConfig));
}

void URPGHeroComponent::AddInputMappingContext(UInputMappingContext* MappingContext, int32 Priority)
{
	if (!MappingContext)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		return;
	}

	if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(MappingContext, Priority);
			CurrentInputMappingContext = MappingContext;
			CurrentIMCPriority = Priority;

			UE_LOG(LogRPG, Log, TEXT("URPGHeroComponent::AddInputMappingContext - Added IMC at priority %d"), Priority);
		}
	}
}

void URPGHeroComponent::RemoveInputMappingContext(UInputMappingContext* MappingContext)
{
	if (!MappingContext)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		return;
	}

	if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->RemoveMappingContext(MappingContext);
			CurrentInputMappingContext = nullptr;

			UE_LOG(LogRPG, Log, TEXT("URPGHeroComponent::RemoveInputMappingContext - Removed IMC"));
		}
	}
}
