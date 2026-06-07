// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RPGCameraMode.h"

#include "RPGCameraMode_TopDown.generated.h"

#define UE_API RPGGAME_API

/**
 * URPGCameraMode_TopDown
 *
 *   俯视角相机模式（黑帝斯 / 死亡细胞 风格，紧跟玩家、固定俯角、滚轮缩放）。
 *
 *   设计参考：02_战斗操作与能量决策.md 决策 #5/#6（战斗中不用鼠标、移动方向 = 朝向）。
 *   不做 RTS 屏幕边缘移相机；不做相机 Yaw 跟随玩家朝向。
 *
 *   配置项：
 *     FieldOfView          —— 视野角度（默认 60°）
 *     CameraPitch          —— 俯角（默认 -60°，0 = 水平视角，-90 = 正俯视）
 *     CameraYaw            —— 固定 Yaw（默认 0°；不跟随玩家朝向，避免眩晕）
 *     TargetArmLength      —— 当前距离（运行时由滚轮 IA 修改）
 *     DefaultArmLength     —— 初始距离（默认 1200）
 *     MinArmLength         —— 滚轮拉近极限（默认 800）
 *     MaxArmLength         —— 滚轮拉远极限（默认 2000）
 *     ZoomInterpSpeed      —— 滚轮缩放平滑速度（默认 8.0，0 = 立即生效）
 */
UCLASS(MinimalAPI, Abstract, Blueprintable)
class URPGCameraMode_TopDown : public URPGCameraMode
{
	GENERATED_BODY()

public:

	UE_API URPGCameraMode_TopDown();

	/** 由 IA_CameraZoom 调用，调整目标 ArmLength（Delta 是单次滚轮量 × ScrollSpeed）。 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Camera|TopDown")
	UE_API void AddZoomDelta(float Delta);

	/** 直接设置目标距离（绝对值，钳制在 [MinArmLength, MaxArmLength]）。 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Camera|TopDown")
	UE_API void SetTargetArmLength(float NewArmLength);

	/** 当前实际距离。 */
	UFUNCTION(BlueprintPure, Category = "RPG|Camera|TopDown")
	float GetCurrentArmLength() const { return CurrentArmLength; }

protected:

	UE_API virtual void UpdateView(float DeltaTime) override;

	// === 视角参数 ===

	/** 俯角（度，-90 ~ 0；默认 -60° = 朝下 60°）。 */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown", meta = (UIMin = "-89.9", UIMax = "0.0", ClampMin = "-89.9", ClampMax = "0.0"))
	float CameraPitch;

	/** 固定 Yaw（度；默认 0° = 朝 +X）。不跟随玩家旋转。 */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown")
	float CameraYaw;

	// === 距离参数 ===

	/** 默认距离（cm）。 */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown", meta = (UIMin = "300.0", ClampMin = "100.0"))
	float DefaultArmLength;

	/** 滚轮拉近极限（cm）。 */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown", meta = (UIMin = "100.0", ClampMin = "100.0"))
	float MinArmLength;

	/** 滚轮拉远极限（cm）。 */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown", meta = (UIMin = "300.0", ClampMin = "300.0"))
	float MaxArmLength;

	/** 缩放平滑速度（FInterpTo Speed；0 = 立即生效）。 */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown", meta = (UIMin = "0.0", ClampMin = "0.0"))
	float ZoomInterpSpeed;

	// === Pivot 偏移（让相机看向角色头顶上方一点，让玩家在屏幕中下半区） ===

	/** Pivot 在角色身上的偏移（默认 (0,0,80) = 头顶；用于把目光焦点抬高，俯视角时玩家不会被脚下场景挡）。 */
	UPROPERTY(EditDefaultsOnly, Category = "TopDown")
	FVector PivotOffset;

	// === 运行时状态 ===

	/** 目标 ArmLength（被 AddZoomDelta 修改；运行时插值到此值）。 */
	UPROPERTY(Transient)
	float TargetArmLength;

	/** 当前实际 ArmLength（每帧 InterpTo 到 TargetArmLength）。 */
	UPROPERTY(Transient)
	float CurrentArmLength;

	/** 是否已初始化（首次 UpdateView 时把 Current/Target 设为 DefaultArmLength）。 */
	UPROPERTY(Transient)
	bool bInitialized;
};

#undef UE_API
