// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGCameraMode.h"
#include "Camera/RPGCameraComponent.h"
#include "System/RPGLogChannels.h"
#include "Engine/Canvas.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGCameraMode)

URPGCameraMode::URPGCameraMode()
{
}

void URPGCameraMode::OnActivate()
{
	ActivationTime = 0.0f;
}

void URPGCameraMode::OnDeactivate()
{
}

void URPGCameraMode::UpdateCameraMode(float DeltaTime)
{
	ActivationTime += DeltaTime;

	// 基类默认实现：将相机定位到目标位置 + 偏移量。
	// 子类（如 TopDown）应重写此方法计算自定义 ViewLocation/ViewRotation。
	AActor* TargetActor = GetTargetActor();
	if (TargetActor)
	{
		ViewLocation = GetCameraTargetLocation() + CameraOffset;
		ViewRotation = FRotator(ViewPitch, ViewYaw, ViewRoll);
	}
}

AActor* URPGCameraMode::GetTargetActor() const
{
	if (CameraComponent)
	{
		if (AActor* Owner = CameraComponent->GetOwner())
		{
			// If the owner is a pawn, use the controller's pawn
			if (APawn* Pawn = Cast<APawn>(Owner))
			{
				return Pawn;
			}
			return Owner;
		}
	}
	return nullptr;
}

FVector URPGCameraMode::GetCameraTargetLocation() const
{
	if (const AActor* TargetActor = GetTargetActor())
	{
		return TargetActor->GetActorLocation();
	}
	return FVector::ZeroVector;
}

void URPGCameraMode::DrawDebug(UCanvas* Canvas) const
{
	// Default: no debug drawing
}
