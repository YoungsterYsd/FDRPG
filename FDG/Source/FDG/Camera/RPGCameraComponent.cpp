// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGCameraComponent.h"
#include "Camera/RPGCameraMode.h"
#include "Camera/RPGCameraModeStack.h"
#include "Character/RPGPawnData.h"
#include "Character/RPGPawnExtensionComponent.h"
#include "System/RPGLogChannels.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraShakeBase.h"
#include "Engine/Engine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGCameraComponent)

URPGCameraComponent::URPGCameraComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void URPGCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogRPG, Log, TEXT("URPGCameraComponent::BeginPlay - Owner: %s"), *GetNameSafe(GetOwner()));

	// Create the camera mode stack
	CameraModeStack = NewObject<URPGCameraModeStack>(this);
	if (CameraModeStack)
	{
		CameraModeStack->SetCameraComponent(this);
	}

	// Push the default camera mode
	TryPushDefaultCameraMode();
}

void URPGCameraComponent::TryPushDefaultCameraMode()
{
	if (CameraModeStack && CameraModeStack->GetTopCameraMode() != nullptr)
	{
		// Already have a camera mode pushed
		return;
	}

	if (TSubclassOf<URPGCameraMode> DefaultMode = GetDefaultCameraMode())
	{
		PushCameraMode(DefaultMode);
		UE_LOG(LogRPG, Log, TEXT("URPGCameraComponent - Pushed default camera mode: %s"), *DefaultMode->GetName());
	}
	else
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGCameraComponent - No default camera mode available yet, will retry on tick"));
	}
}

void URPGCameraComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCamera(DeltaTime);
}

void URPGCameraComponent::UpdateCamera(float DeltaTime)
{
	if (!CameraModeStack)
	{
		return;
	}

	// If no camera mode is pushed yet, try to push the default
	if (CameraModeStack->GetTopCameraMode() == nullptr)
	{
		TryPushDefaultCameraMode();
		if (CameraModeStack->GetTopCameraMode() == nullptr)
		{
			return;
		}
	}

	// Evaluate the camera mode stack
	FVector StackLocation;
	FRotator StackRotation;
	float StackFOV;

	CameraModeStack->UpdateStack(DeltaTime, StackLocation, StackRotation, StackFOV);

	CameraLocation = StackLocation;
	CameraRotation = StackRotation;
	FieldOfView = StackFOV;

	// 将相机旋转写入 ControlRotation，使移动方向相对相机朝向
	// 实际相机画面通过 ARPGCharacter::CalcCamera 读取缓存的 CameraLocation/CameraRotation/FieldOfView
	if (AActor* Owner = GetOwner())
	{
		if (APawn* Pawn = Cast<APawn>(Owner))
		{
			if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
			{
				PC->SetControlRotation(CameraRotation);
			}
		}
	}
}

URPGCameraComponent* URPGCameraComponent::FindCameraComponent(const AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	return Actor->FindComponentByClass<URPGCameraComponent>();
}

URPGCameraMode* URPGCameraComponent::PushCameraMode(TSubclassOf<URPGCameraMode> CameraModeClass)
{
	if (!CameraModeStack)
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGCameraComponent::PushCameraMode - CameraModeStack is null"));
		return nullptr;
	}

	return CameraModeStack->PushCameraMode(CameraModeClass);
}

TSubclassOf<URPGCameraMode> URPGCameraComponent::GetDefaultCameraMode() const
{
	if (const AActor* Owner = GetOwner())
	{
		if (const URPGPawnExtensionComponent* PawnExt = URPGPawnExtensionComponent::FindPawnExtensionComponent(Owner))
		{
			if (const URPGPawnData* PawnData = PawnExt->GetPawnData())
			{
				if (PawnData->DefaultCameraMode.IsValid())
				{
					return PawnData->DefaultCameraMode.Get();
				}
				// Try synchronous load if the soft reference has a path but isn't loaded yet
				if (!PawnData->DefaultCameraMode.IsNull())
				{
					if (TSubclassOf<URPGCameraMode> LoadedMode = PawnData->DefaultCameraMode.LoadSynchronous())
					{
						UE_LOG(LogRPG, Log, TEXT("URPGCameraComponent - LoadSynchronous succeeded for DefaultCameraMode: %s"), *LoadedMode->GetName());
						return LoadedMode;
					}
					else
					{
						UE_LOG(LogRPG, Warning, TEXT("URPGCameraComponent - LoadSynchronous FAILED for DefaultCameraMode path: %s"), *PawnData->DefaultCameraMode.ToString());
					}
				}
				else
				{
					UE_LOG(LogRPG, Warning, TEXT("URPGCameraComponent - PawnData '%s' has NO DefaultCameraMode set!"), *GetNameSafe(PawnData));
				}
			}
			else
			{
				UE_LOG(LogRPG, Warning, TEXT("URPGCameraComponent - PawnExtensionComponent has no PawnData yet"));
			}
		}
		else
		{
			UE_LOG(LogRPG, Warning, TEXT("URPGCameraComponent - No PawnExtensionComponent on owner '%s'"), *GetNameSafe(Owner));
		}
	}
	return nullptr;
}

void URPGCameraComponent::StartCameraShake(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale)
{
	if (AActor* Owner = GetOwner())
	{
		if (APawn* Pawn = Cast<APawn>(Owner))
		{
			if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
			{
				PC->ClientStartCameraShake(ShakeClass, Scale);
			}
		}
	}
}

void URPGCameraComponent::StopAllCameraShakes()
{
	// UE5.7: ClientStopCameraShakes was removed; use ClientStartCameraShake with null to stop
	// Camera shake stopping is handled by the PlayerController's camera manager
	if (AActor* Owner = GetOwner())
	{
		if (APawn* Pawn = Cast<APawn>(Owner))
		{
			if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
			{
				if (PC->PlayerCameraManager)
				{
					PC->PlayerCameraManager->StopAllCameraShakes();
				}
			}
		}
	}
}
