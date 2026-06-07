// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGCameraMode_TopDown.h"

#include "Camera/RPGCameraComponent.h"
#include "GameFramework/Actor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGCameraMode_TopDown)

URPGCameraMode_TopDown::URPGCameraMode_TopDown()
	: CameraPitch(-60.0f)
	, CameraYaw(0.0f)
	, DefaultArmLength(1200.0f)
	, MinArmLength(800.0f)
	, MaxArmLength(2000.0f)
	, ZoomInterpSpeed(8.0f)
	, PivotOffset(0.0f, 0.0f, 80.0f)
	, TargetArmLength(1200.0f)
	, CurrentArmLength(1200.0f)
	, bInitialized(false)
{
	// 父类的 ViewPitchMin/Max 在俯视角下没意义（玩家不能用鼠标改 Pitch），但为了 Blend 兼容保留默认。
	FieldOfView = 60.0f;
}

void URPGCameraMode_TopDown::AddZoomDelta(float Delta)
{
	SetTargetArmLength(TargetArmLength + Delta);
}

void URPGCameraMode_TopDown::SetTargetArmLength(float NewArmLength)
{
	TargetArmLength = FMath::Clamp(NewArmLength, MinArmLength, MaxArmLength);
}

void URPGCameraMode_TopDown::UpdateView(float DeltaTime)
{
	const AActor* TargetActor = GetTargetActor();
	if (!TargetActor)
	{
		Super::UpdateView(DeltaTime);
		return;
	}

	// 首次进入：把 Current/Target 锁到默认值（避免编辑器热重载时 Transient 字段为 0）。
	if (!bInitialized)
	{
		TargetArmLength = FMath::Clamp(DefaultArmLength, MinArmLength, MaxArmLength);
		CurrentArmLength = TargetArmLength;
		bInitialized = true;
	}

	// 消费 IA_CameraZoom 累加的 PendingZoomDelta（在 RPGCameraComponent 上）
	if (URPGCameraComponent* CameraComp = GetRPGCameraComponent())
	{
		const float PendingDelta = CameraComp->ConsumePendingZoomDelta();
		if (!FMath::IsNearlyZero(PendingDelta))
		{
			AddZoomDelta(PendingDelta);
		}
	}

	// 1) Pivot：角色位置 + PivotOffset
	const FVector PivotLocation = TargetActor->GetActorLocation() + PivotOffset;

	// 2) Pivot 旋转：固定 Pitch/Yaw，不跟玩家朝向
	const FRotator PivotRotation(CameraPitch, CameraYaw, 0.0f);

	// 3) 平滑缩放
	if (ZoomInterpSpeed > KINDA_SMALL_NUMBER)
	{
		CurrentArmLength = FMath::FInterpTo(CurrentArmLength, TargetArmLength, DeltaTime, ZoomInterpSpeed);
	}
	else
	{
		CurrentArmLength = TargetArmLength;
	}

	// 4) 计算相机位置：从 Pivot 沿"反方向"（朝相机自己）退一段距离
	//    Forward 方向 = PivotRotation.Vector() = 相机看向角色的方向
	//    所以相机位置 = Pivot - Forward * ArmLength
	const FVector CameraOffset = PivotRotation.Vector() * CurrentArmLength;
	const FVector CameraLocation = PivotLocation - CameraOffset;

	// 5) 输出
	View.Location = CameraLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = PivotRotation;
	View.FieldOfView = FieldOfView;
}

