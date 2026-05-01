// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "RPGCameraMode.generated.h"

class URPGCameraComponent;

/**
 * ERPGCameraModeBlendFunction
 *
 *	相机模式切换混合函数。
 */
UENUM(BlueprintType)
enum class ERPGCameraModeBlendFunction : uint8
{
	// Linear blend
	Linear,

	// Ease in (slow start, fast end)
	EaseIn,

	// Ease out (fast start, slow end)
	EaseOut,

	// Ease in and out (slow start and end, fast middle)
	EaseInOut,
};

/**
 * URPGCameraMode
 *
 *	相机模式基类，定义相机参数（FOV、位置、旋转）和混合策略。
 *	由 CameraModeStack 实例化并每帧更新。
 */
UCLASS(Abstract, Blueprintable, Meta = (ShortTooltip = "Base class for camera modes."))
class RPGGAME_API URPGCameraMode : public UObject
{
	GENERATED_BODY()

public:

	URPGCameraMode();

	// Called when this camera mode is activated
	virtual void OnActivate();

	// Called when this camera mode is deactivated
	virtual void OnDeactivate();

	// Update the camera mode each frame
	virtual void UpdateCameraMode(float DeltaTime);

	// Get the target actor that the camera should follow
	AActor* GetTargetActor() const;

	// Get the target location for the camera to follow
	virtual FVector GetCameraTargetLocation() const;

	// Draw debug visualization
	virtual void DrawDebug(UCanvas* Canvas) const;

	// The computed view location (set by derived camera modes in UpdateCameraMode)
	FVector ViewLocation = FVector::ZeroVector;

	// The computed view rotation (set by derived camera modes in UpdateCameraMode)
	FRotator ViewRotation = FRotator::ZeroRotator;

	// Field of view for this camera mode
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float FieldOfView = 90.0f;

	// View pitch
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float ViewPitch = -45.0f;

	// View yaw
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float ViewYaw = 0.0f;

	// View roll
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float ViewRoll = 0.0f;

	// Blend time when switching to this camera mode
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float BlendTime = 0.5f;

	// Blend function for camera mode transitions
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	ERPGCameraModeBlendFunction BlendFunction = ERPGCameraModeBlendFunction::EaseOut;

	// Blend exponent (used for EaseIn/EaseOut/EaseInOut)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float BlendExponent = 2.0f;

	// Camera offset from the target
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	FVector CameraOffset = FVector::ZeroVector;

	// The current blend weight (0.0 to 1.0)
	float GetBlendWeight() const { return BlendAlpha; }

	// Set the blend alpha (used by CameraModeStack)
	void SetBlendAlpha(float InAlpha) { BlendAlpha = InAlpha; }

	// The camera component that owns this mode
	void SetCameraComponent(URPGCameraComponent* InCameraComponent) { CameraComponent = InCameraComponent; }
	URPGCameraComponent* GetCameraComponent() const { return CameraComponent; }

	// Time elapsed since this mode was activated
	float GetActivationTime() const { return ActivationTime; }

protected:

	// The camera component that owns this mode
	UPROPERTY(Transient)
	TObjectPtr<URPGCameraComponent> CameraComponent;

	// The current blend alpha (0.0 to 1.0)
	float BlendAlpha = 1.0f;

	// Time elapsed since activation
	float ActivationTime = 0.0f;
};

/**
 * FRPGCameraModeStackEntry
 *
 *	Entry in the camera mode stack.
 */
USTRUCT()
struct FRPGCameraModeStackEntry
{
	GENERATED_BODY()

public:

	// The camera mode instance
	UPROPERTY(Transient)
	TObjectPtr<URPGCameraMode> CameraMode = nullptr;

	// The blend time remaining
	float BlendTimeRemaining = 0.0f;

	// The total blend duration
	float TotalBlendTime = 0.0f;
};
