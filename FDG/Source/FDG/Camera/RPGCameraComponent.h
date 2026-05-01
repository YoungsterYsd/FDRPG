// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "RPGCameraComponent.generated.h"

class URPGCameraMode;
class URPGCameraModeStack;

/**
 * URPGCameraComponent
 *
 *	RPG 角色相机组件，持有 CameraModeStack 并提供相机模式切换。
 *	每帧评估相机模式堆栈，确定最终相机变换（位置/旋转/FOV）。
 *	通过 ARPGCharacter::CalcCamera 将结果输出到 PlayerCameraManager。
 */
UCLASS(Meta = (BlueprintSpawnableComponent))
class RPGGAME_API URPGCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	URPGCameraComponent();

	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~End of UActorComponent interface

	// Returns the camera component if found on the specified actor
	static URPGCameraComponent* FindCameraComponent(const AActor* Actor);

	// Push a camera mode onto the stack
	URPGCameraMode* PushCameraMode(TSubclassOf<URPGCameraMode> CameraModeClass);

	// Get the default camera mode from PawnData
	TSubclassOf<URPGCameraMode> GetDefaultCameraMode() const;

	// Get the camera mode stack
	URPGCameraModeStack* GetCameraModeStack() const { return CameraModeStack; }

	// Get the current camera location
	FVector GetCameraLocation() const { return CameraLocation; }

	// Get the current camera rotation
	FRotator GetCameraRotation() const { return CameraRotation; }

	// Get the current field of view
	float GetFieldOfView() const { return FieldOfView; }

	// Start a camera shake
	void StartCameraShake(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale = 1.0f);

	// Stop all camera shakes
	void StopAllCameraShakes();

protected:

	// Update the camera from the camera mode stack
	virtual void UpdateCamera(float DeltaTime);

	// Try to push the default camera mode from PawnData
	void TryPushDefaultCameraMode();

private:

	// The camera mode stack
	UPROPERTY(Transient)
	TObjectPtr<URPGCameraModeStack> CameraModeStack;

	// Current camera parameters (updated each frame from the mode stack)
	FVector CameraLocation = FVector::ZeroVector;
	FRotator CameraRotation = FRotator::ZeroRotator;
	float FieldOfView = 90.0f;
};
