// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/RPGCameraMode.h"
#include "RPGCameraMode_TopDown.generated.h"

/**
 * URPGCameraMode_TopDown
 *
 *	俯视角相机模式，相机从上方跟随目标角色。
 *	所有参数暴露为 Blueprint 可编辑，便于实时调优。
 */
UCLASS(Blueprintable, Meta = (DisplayName = "RPG Camera Mode - Top Down"))
class RPGGAME_API URPGCameraMode_TopDown : public URPGCameraMode
{
	GENERATED_BODY()

public:

	URPGCameraMode_TopDown();

	//~URPGCameraMode interface
	virtual void OnActivate() override;
	virtual void UpdateCameraMode(float DeltaTime) override;
	//~End of URPGCameraMode interface

protected:

	// Camera height above the target
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TopDown")
	float CameraHeight = 800.0f;

	// Camera angle pitch (negative = looking down)
	// Overrides the base class ViewPitch for top-down specific pitch
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TopDown")
	float CameraAnglePitch = -45.0f;

	// Camera follow interpolation speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopDown")
	float FollowInterpSpeed = 10.0f;

	// Whether to enable smooth follow interpolation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopDown")
	bool bEnableFollowInterp = true;

	// Horizontal distance from target (follow distance)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopDown")
	float FollowDistance = 0.0f;

private:

	// The current smoothed target location
	FVector SmoothedTargetLocation = FVector::ZeroVector;

	// Whether the target location has been initialized
	bool bTargetLocationInitialized = false;
};
