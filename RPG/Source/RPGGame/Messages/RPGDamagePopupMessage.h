// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "RPGDamagePopupMessage.generated.h"

/**
 * 飘字消息（A3 v7）
 *
 *  由 URPGDamageExecution 在伤害结算后通过 GameplayMessageSubsystem 广播 RPG.Message.Damage.Popup。
 *  订阅方：URPGDamageNumberSubsystem 接收后创建 WBP_RPG_DamageNumber Widget 投影到屏幕。
 *
 *  4 种样式由订阅端根据本结构字段判断：
 *    - IsCrit == true   → 暴击（金 40 + 描边）
 *    - Amount < 0       → 治疗（绿 28 + "+" 前缀，按惯例由 Healing 通道触发，Amount 取负）
 *    - IsBlocked == true→ 格挡（灰 24 + "BLOCK" 文字）
 *    - 其他             → 普通（白 28）
 */
USTRUCT(BlueprintType)
struct FRPGDamagePopupMessage
{
	GENERATED_BODY()

	/** 攻击发起者 Actor。 */
	UPROPERTY(BlueprintReadWrite, Category = "RPG|Damage")
	TObjectPtr<UObject> Instigator = nullptr;

	/** 受击 Actor。 */
	UPROPERTY(BlueprintReadWrite, Category = "RPG|Damage")
	TObjectPtr<UObject> Target = nullptr;

	/** 数值（伤害正数，治疗负数；MISS/Block 0 也允许）。 */
	UPROPERTY(BlueprintReadWrite, Category = "RPG|Damage")
	float Amount = 0.0f;

	/** 是否暴击。 */
	UPROPERTY(BlueprintReadWrite, Category = "RPG|Damage")
	bool bIsCrit = false;

	/** 是否被普通格挡（v7：完美格挡走单独 Message_Block_PerfectTriggered）。 */
	UPROPERTY(BlueprintReadWrite, Category = "RPG|Damage")
	bool bIsBlocked = false;

	/** 命中位置（用于 IndicatorSystem 屏幕投影；如无则为 ZeroVector，订阅端 fallback 到 Target 位置）。 */
	UPROPERTY(BlueprintReadWrite, Category = "RPG|Damage")
	FVector HitLocation = FVector::ZeroVector;
};
