// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/Abilities/RPGGameplayAbility.h"

#include "RPGGameplayAbility_Block.generated.h"

#define UE_API RPGGAME_API

/**
 * URPGGameplayAbility_Block（A3 v7 空壳）
 *
 *  A3 阶段仅声明类 + 4 个配置 UPROPERTY + Print Log 占位实现。
 *  完整逻辑在 A5 实现：
 *    - Activate: Apply GE_Block_State (SetByCaller: BlockDamageReduction / BlockStaminaConsumeRate)
 *                + 加 Tag State.Block.Blocking
 *                + 启动 PerfectBlockWindow Timer（窗口期内加 State.Block.PerfectBlocking）
 *    - Tick:    检查 StaminaCurrent 耗尽 / 玩家松键
 *    - End:     移除 Tag + 移除 GE
 *    - 订阅 RPG.Message.Block.PerfectTriggered → Apply 耐力+20 GE + 强化下次攻击 Buff GE
 *
 *  4 个配置字段都标 EditDefaultsOnly：
 *    - 默认值在 BP 子类 / 武器派生 GA 中可改（盾牌强减伤、法杖弱减伤）
 *    - 词条/Boon 也可通过 BP CDO 修改
 *
 *  详见 14_后续日程与验收清单.md §3.8
 */
UCLASS(MinimalAPI, Abstract)
class URPGGameplayAbility_Block : public URPGGameplayAbility
{
	GENERATED_BODY()

public:

	UE_API URPGGameplayAbility_Block(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 完美格挡判定窗口（按下后 N 秒内被命中 → 完美格挡）单位：秒。02 §F.3 锁定 0.3。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Block", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float PerfectBlockWindow = 0.3f;

	/** 普通格挡减伤系数（0..1）。02 §F.3 锁定 0.5（50% 减伤）。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Block", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float BlockDamageReduction = 0.5f;

	/** 普通格挡耐力消耗系数（消耗 = finalDamage × 该值）。02 §F.3 锁定 0.6。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Block", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float BlockStaminaConsumeRate = 0.6f;

	/** 破防（StaminaCurrent=0）后硬直时长（秒）。02 §F.3 锁定 1.2s。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Block", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float BlockBrokenStunDuration = 1.2f;

protected:

	/** A3 空壳实现：仅 Print Log 占位，A5 实现完整逻辑。 */
	UE_API virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UE_API virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;
};

#undef UE_API
