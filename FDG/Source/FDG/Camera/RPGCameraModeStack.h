// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Camera/RPGCameraMode.h"
#include "RPGCameraModeStack.generated.h"

class URPGCameraComponent;

/**
 * URPGCameraModeStack
 *
 *	相机模式堆栈，管理多个相机模式的混合。
 *	每帧评估所有活跃的相机模式并混合，生成最终相机参数。
 *	堆栈底部是基础相机模式（通常是默认模式），新模式在顶部混合叠加。
 */
UCLASS()
class RPGGAME_API URPGCameraModeStack : public UObject
{
	GENERATED_BODY()

public:

	URPGCameraModeStack();

	// Push a new camera mode onto the stack
	URPGCameraMode* PushCameraMode(TSubclassOf<URPGCameraMode> CameraModeClass);

	// Evaluate the stack and return the blended camera parameters
	void UpdateStack(float DeltaTime, FVector& OutCameraLocation, FRotator& OutCameraRotation, float& OutFOV);

	// Get the camera component that owns this stack
	void SetCameraComponent(URPGCameraComponent* InCameraComponent) { CameraComponent = InCameraComponent; }
	URPGCameraComponent* GetCameraComponent() const { return CameraComponent; }

	// Draw debug visualization
	void DrawDebug(UCanvas* Canvas) const;

	// Get the top camera mode
	URPGCameraMode* GetTopCameraMode() const;

	// Get the number of camera modes in the stack
	int32 GetNumCameraModes() const { return CameraModeStack.Num(); }

private:

	// Create a camera mode instance
	URPGCameraMode* CreateCameraMode(TSubclassOf<URPGCameraMode> CameraModeClass);

	// The camera component that owns this stack
	UPROPERTY(Transient)
	TObjectPtr<URPGCameraComponent> CameraComponent;

	// The stack of camera mode entries
	UPROPERTY(Transient)
	TArray<FRPGCameraModeStackEntry> CameraModeStack;
};
