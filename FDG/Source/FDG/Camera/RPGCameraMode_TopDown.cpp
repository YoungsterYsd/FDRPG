// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGCameraMode_TopDown.h"
#include "Camera/RPGCameraComponent.h"
#include "System/RPGLogChannels.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGCameraMode_TopDown)

URPGCameraMode_TopDown::URPGCameraMode_TopDown()
{
	// Set default top-down camera parameters
	FieldOfView = 90.0f;
	ViewPitch = -45.0f;
	ViewYaw = 0.0f;
	ViewRoll = 0.0f;
	BlendTime = 0.5f;
	CameraOffset = FVector::ZeroVector;
}

void URPGCameraMode_TopDown::OnActivate()
{
	Super::OnActivate();

	UE_LOG(LogRPG, Display, TEXT("URPGCameraMode_TopDown::OnActivate - Class: %s, CameraHeight: %.1f, CameraAnglePitch: %.1f, FollowDistance: %.1f, FollowInterpSpeed: %.1f"),
		*GetClass()->GetName(), CameraHeight, CameraAnglePitch, FollowDistance, FollowInterpSpeed);
}

void URPGCameraMode_TopDown::UpdateCameraMode(float DeltaTime)
{
	Super::UpdateCameraMode(DeltaTime);

	AActor* TargetActor = GetTargetActor();
	if (!TargetActor)
	{
		return;
	}

	// 1. Get target location
	FVector TargetLocation = GetCameraTargetLocation();

	// 2. Initialize or smooth follow
	if (!bTargetLocationInitialized)
	{
		SmoothedTargetLocation = TargetLocation;
		bTargetLocationInitialized = true;
	}
	else if (bEnableFollowInterp)
	{
		SmoothedTargetLocation = FMath::VInterpTo(SmoothedTargetLocation, TargetLocation, DeltaTime, FollowInterpSpeed);
	}
	else
	{
		SmoothedTargetLocation = TargetLocation;
	}

	// 3. 根据高度和角度计算相机位置
	const float EffectivePitch = CameraAnglePitch;
	const float PitchRadians = FMath::DegreesToRadians(EffectivePitch);

	// 水平偏移 = 高度 × tan(PI/2 + 俯角)
	// 使用硬编码常量避免 Windows.h 的 PI 宏冲突
	const float HalfPi = 1.57079632679489661923f;
	const float HorizontalOffset = CameraHeight * FMath::Tan(HalfPi + PitchRadians);

	// Get the control rotation yaw from the controller for camera direction
	FRotator ControlRotation(0.0f, 0.0f, 0.0f);
	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		if (const AController* Controller = TargetPawn->GetController())
		{
			ControlRotation = Controller->GetControlRotation();
		}
	}

	// Camera position: behind and above the target based on control rotation
	const FRotator CameraYawRotation(0.0f, ControlRotation.Yaw, 0.0f);
	const FVector ForwardDir = CameraYawRotation.RotateVector(FVector::ForwardVector);

	// Offset the camera backwards (behind the target) and add follow distance
	FVector CameraLocation = SmoothedTargetLocation;
	CameraLocation += ForwardDir * (-HorizontalOffset + FollowDistance);
	CameraLocation.Z += CameraHeight;

	// Apply additional offset from base class
	CameraLocation += CameraOffset;

	// 4. Calculate camera rotation (looking at target)
	const FVector Direction = SmoothedTargetLocation - CameraLocation;
	FRotator CameraRotation = Direction.Rotation();

	// 5. Store the computed transform for the CameraModeStack to read
	ViewLocation = CameraLocation;
	ViewRotation = CameraRotation;
}
