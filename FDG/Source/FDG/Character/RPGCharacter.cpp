// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGCharacter.h"
#include "Character/RPGPawnExtensionComponent.h"
#include "Character/RPGPawnData.h"
#include "Camera/RPGCameraComponent.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Player/RPGPlayerState.h"
#include "GameplayTags/RPGGameplayTags.h"
#include "System/RPGAssetManager.h"
#include "System/RPGLogChannels.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGCharacter)

ARPGCharacter::ARPGCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 角色朝向由移动方向决定（bOrientRotationToMovement），不使用 Controller 旋转
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PawnExtensionComponent = CreateDefaultSubobject<URPGPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	CameraComponent = CreateDefaultSubobject<URPGCameraComponent>(TEXT("CameraComponent"));
}

void ARPGCharacter::PostInitializeComponents()
{
	UE_LOG(LogRPG, Log, TEXT("ARPGCharacter::PostInitializeComponents - %s"), *GetName());

	Super::PostInitializeComponents();

	// 从类属性写入 CharacterMovementComponent 参数
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = MaxWalkSpeed;
		MoveComp->MaxAcceleration = MaxAcceleration;
		MoveComp->BrakingDecelerationWalking = BrakingDecelerationWalking;
		MoveComp->BrakingFriction = BrakingFriction;
		MoveComp->bOrientRotationToMovement = true;
		MoveComp->bUseControllerDesiredRotation = false;
		MoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	}

	InitPawnData();
}

void ARPGCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnAbilitySystemUninitialized();

	Super::EndPlay(EndPlayReason);
}

void ARPGCharacter::Reset()
{
	Super::Reset();

	// Reset the pawn extension component
	if (PawnExtensionComponent)
	{
		PawnExtensionComponent->OnAbilitySystemUninitialized();
	}

	bAbilitySystemInitialized = false;
}

void ARPGCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Initialize the ASC with this character as the avatar (server path)
	// Client path is handled in OnRep_PlayerState
	OnAbilitySystemInitialized();

	// Notify the pawn extension component that the controller changed
	if (PawnExtensionComponent)
	{
		PawnExtensionComponent->HandleControllerChanged();
	}
}

void ARPGCharacter::UnPossessed()
{
	Super::UnPossessed();

	// Notify the pawn extension component that the controller changed
	if (PawnExtensionComponent)
	{
		PawnExtensionComponent->HandleControllerChanged();
	}
}

void ARPGCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Notify the pawn extension component that the player state changed
	if (PawnExtensionComponent)
	{
		PawnExtensionComponent->OnPlayerStateChanged();
	}

	// Initialize ability system on the client
	OnAbilitySystemInitialized();
}

void ARPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Delegate input setup to the PawnExtensionComponent, which will
	// forward it to the HeroComponent on the PlayerController
	if (PawnExtensionComponent)
	{
		PawnExtensionComponent->SetupPlayerInputComponent();
	}
}

UAbilitySystemComponent* ARPGCharacter::GetAbilitySystemComponent() const
{
	return GetRPGAbilitySystemComponent();
}

URPGAbilitySystemComponent* ARPGCharacter::GetRPGAbilitySystemComponent() const
{
	if (const ARPGPlayerState* RPGPS = GetPlayerState<ARPGPlayerState>())
	{
		return RPGPS->GetRPGAbilitySystemComponent();
	}
	return nullptr;
}

void ARPGCharacter::Move(const FInputActionValue& InputActionValue)
{
	if (!Controller)
	{
		return;
	}

	const FVector2D MoveVector = InputActionValue.Get<FVector2D>();

	// 俯视角：移动方向相对相机朝向（上=屏幕上方）
	// 箭头键上(Y=1) → 角色沿相机前方移动
	const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
	AddMovementInput(MovementRotation.RotateVector(FVector::ForwardVector), MoveVector.Y);
	AddMovementInput(MovementRotation.RotateVector(FVector::RightVector), MoveVector.X);
}

void ARPGCharacter::OnAbilitySystemInitialized()
{
	if (bAbilitySystemInitialized)
	{
		return;
	}

	if (URPGAbilitySystemComponent* ASC = GetRPGAbilitySystemComponent())
	{
		bAbilitySystemInitialized = true;

		// Initialize the ASC actor info if needed
		if (!ASC->GetAvatarActor())
		{
			ASC->InitAbilityActorInfo(GetPlayerState(), this);
		}

		UE_LOG(LogRPG, Log, TEXT("ARPGCharacter::OnAbilitySystemInitialized - ASC initialized for %s"), *GetName());
	}
}

void ARPGCharacter::OnAbilitySystemUninitialized()
{
	if (!bAbilitySystemInitialized)
	{
		return;
	}

	bAbilitySystemInitialized = false;
}

void ARPGCharacter::InitPawnData()
{
	UE_LOG(LogRPG, Log, TEXT("ARPGCharacter::InitPawnData - Called for %s"), *GetName());

	if (!PawnExtensionComponent)
	{
		UE_LOG(LogRPG, Error, TEXT("ARPGCharacter::InitPawnData - PawnExtensionComponent is null!"));
		return;
	}

	if (PawnExtensionComponent->GetPawnData())
	{
		UE_LOG(LogRPG, Log, TEXT("ARPGCharacter::InitPawnData - PawnData already set"));
		return;
	}

	if (const URPGPawnData* DefaultPawnData = URPGAssetManager::Get().GetDefaultPawnData())
	{
		UE_LOG(LogRPG, Log, TEXT("ARPGCharacter::InitPawnData - Got DefaultPawnData: %s"), *GetNameSafe(DefaultPawnData));
		PawnExtensionComponent->SetPawnData(DefaultPawnData);
	}
	else
	{
		// GameFeature plugin may not be mounted yet, retry on next frame
		UE_LOG(LogRPG, Warning, TEXT("ARPGCharacter::InitPawnData - DefaultPawnData not found, retrying next frame..."));
		FTimerHandle RetryHandle;
		GetWorldTimerManager().SetTimer(RetryHandle, FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			if (PawnExtensionComponent && !PawnExtensionComponent->GetPawnData())
			{
				if (const URPGPawnData* RetryData = URPGAssetManager::Get().GetDefaultPawnData())
				{
					UE_LOG(LogRPG, Log, TEXT("ARPGCharacter::InitPawnData - Retry succeeded: %s"), *GetNameSafe(RetryData));
					PawnExtensionComponent->SetPawnData(RetryData);
				}
				else
				{
					UE_LOG(LogRPG, Error, TEXT("ARPGCharacter::InitPawnData - Retry FAILED! Check DefaultGame.ini and RPGCore plugin mount status"));
				}
			}
		}), 0.1f, false);
	}
}

void ARPGCharacter::OnDeath()
{
	UE_LOG(LogRPG, Log, TEXT("ARPGCharacter::OnDeath - %s died"), *GetName());

	// P1: Just log, P2 will implement full death handling
	// Detach from controller
	DetachFromControllerPendingDestroy();

	// Disable collision
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Disable movement
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}
}

void ARPGCharacter::OnDeathStarted()
{
	UE_LOG(LogRPG, Log, TEXT("ARPGCharacter::OnDeathStarted - %s"), *GetName());

	// Disable collision on the capsule
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Disable movement
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}
}

void ARPGCharacter::OnDeathFinished()
{
	UE_LOG(LogRPG, Log, TEXT("ARPGCharacter::OnDeathFinished - %s"), *GetName());

	// Hide the character and set lifespan for cleanup
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetLifeSpan(5.0f);
}

void ARPGCharacter::ResetCharacter()
{
	UE_LOG(LogRPG, Log, TEXT("ARPGCharacter::ResetCharacter - %s"), *GetName());

	// Reset health and energy (P8 will implement full respawn)
	if (URPGAbilitySystemComponent* ASC = GetRPGAbilitySystemComponent())
	{
		ASC->RemoveLooseGameplayTag(RPGGameplayTags::Status_Death_Dying);
		ASC->RemoveLooseGameplayTag(RPGGameplayTags::Status_Death_Dead);
	}

	// Re-enable collision and visibility
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	// Re-enable movement
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}
}

void ARPGCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	// Update movement mode tags on the ASC
	if (URPGAbilitySystemComponent* ASC = GetRPGAbilitySystemComponent())
	{
		const EMovementMode CurrentMovementMode = GetCharacterMovement()->MovementMode;

		// Remove old movement mode tag
		if (const FGameplayTag* OldTag = RPGGameplayTags::MovementModeTagMap.Find(PrevMovementMode))
		{
			ASC->RemoveLooseGameplayTag(*OldTag);
		}

		// Add new movement mode tag
		if (const FGameplayTag* NewTag = RPGGameplayTags::MovementModeTagMap.Find(CurrentMovementMode))
		{
			ASC->AddLooseGameplayTag(*NewTag);
		}
	}
}

void ARPGCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	// Delegate to our RPGCameraComponent if available
	if (CameraComponent)
	{
		OutResult.Location = CameraComponent->GetCameraLocation();
		OutResult.Rotation = CameraComponent->GetCameraRotation();
		OutResult.FOV = CameraComponent->GetFieldOfView();
		OutResult.ProjectionMode = ECameraProjectionMode::Perspective;
		OutResult.AspectRatio = 1.7777778f;
		OutResult.bConstrainAspectRatio = true;
		return;
	}

	Super::CalcCamera(DeltaTime, OutResult);
}
