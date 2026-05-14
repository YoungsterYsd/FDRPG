// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "RPGBlockMessage.generated.h"

/**
 * 完美格挡触发消息（A3 v7）
 *
 *  由 URPGDamageExecution 在判定到 Target 处于完美格挡状态时广播 RPG.Message.Block.PerfectTriggered。
 *  订阅方（A5 阶段实现）：
 *    - URPGGameplayAbility_Block 订阅 → Apply 耐力+20 GE + 强化下次攻击 Buff GE
 *    - URPGDamageNumberSubsystem  订阅 → 播 "PERFECT BLOCK!" 飘字
 *    - GameplayCue 订阅           → 播反震特效 + 音效
 *
 *  A3 阶段广播链路通畅 + Print Log 验证即可，无需任何订阅方实际响应。
 */
USTRUCT(BlueprintType)
struct FRPGPerfectBlockMessage
{
	GENERATED_BODY()

	/** 攻击发起者 Actor。 */
	UPROPERTY(BlueprintReadWrite, Category = "RPG|Block")
	TObjectPtr<UObject> Instigator = nullptr;

	/** 完美格挡的 Actor（受击者）。 */
	UPROPERTY(BlueprintReadWrite, Category = "RPG|Block")
	TObjectPtr<UObject> Target = nullptr;

	/** 原本会造成的伤害值（被完美格挡完全免除）。 */
	UPROPERTY(BlueprintReadWrite, Category = "RPG|Block")
	float DamageBlocked = 0.0f;

	/** 攻击来源 Tag（从 GE Spec 的 SourceTags 取得）。 */
	UPROPERTY(BlueprintReadWrite, Category = "RPG|Block")
	FGameplayTagContainer AttackSourceTags;

	/** 命中位置。 */
	UPROPERTY(BlueprintReadWrite, Category = "RPG|Block")
	FVector HitLocation = FVector::ZeroVector;
};
