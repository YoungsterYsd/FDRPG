// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"

#include "RPGCameraComponent.generated.h"

class UCanvas;
class URPGCameraMode;
class URPGCameraModeStack;
class UObject;
struct FFrame;
struct FGameplayTag;
struct FMinimalViewInfo;
template <class TClass> class TSubclassOf;

DECLARE_DELEGATE_RetVal(TSubclassOf<URPGCameraMode>, FRPGCameraModeDelegate);


/**
 * ULyraCameraComponent
 *
 *	The base camera component class used by this project.
 */
UCLASS()
class URPGCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:

	URPGCameraComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the camera component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "RPG|Camera")
	static URPGCameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<URPGCameraComponent>() : nullptr); }

	// Returns the target actor that the camera is looking at.
	virtual AActor* GetTargetActor() const { return GetOwner(); }

	// Delegate used to query for the best camera mode.
	FRPGCameraModeDelegate DetermineCameraModeDelegate;

	// Add an offset to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	void AddFieldOfViewOffset(float FovOffset) { FieldOfViewOffset += FovOffset; }

	/** A4 新增：累加待消费的 ZoomDelta（俯视角相机用，由 IA_CameraZoom 调用，TopDownCameraMode 在 UpdateView 内消费）。 */
	void AddPendingZoomDelta(float ZoomDelta) { PendingZoomDelta += ZoomDelta; }

	/** A4 新增：消费并清零待处理的 ZoomDelta（在 CameraMode UpdateView 内调）。 */
	float ConsumePendingZoomDelta()
	{
		const float Out = PendingZoomDelta;
		PendingZoomDelta = 0.0f;
		return Out;
	}

	virtual void DrawDebug(UCanvas* Canvas) const;

	// Gets the tag associated with the top layer and the blend weight of it
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

protected:

	virtual void OnRegister() override;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	virtual void UpdateCameraModes();

protected:

	// Stack used to blend the camera modes.
	UPROPERTY()
	TObjectPtr<URPGCameraModeStack> CameraModeStack;

	// Offset applied to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	float FieldOfViewOffset;

	/** A4 新增：累加的 ZoomDelta，由 TopDownCameraMode UpdateView 内调 ConsumePendingZoomDelta() 取走。 */
	float PendingZoomDelta = 0.0f;

};
