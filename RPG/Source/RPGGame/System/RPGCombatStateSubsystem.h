// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "UObject/WeakObjectPtr.h"

#include "RPGCombatStateSubsystem.generated.h"

#define UE_API RPGGAME_API

class AActor;
class UAbilitySystemComponent;

/**
 * URPGCombatStateSubsystem（A3-5 v7 空壳）
 *
 *   作用：维护"角色当前是否处于战斗状态"的 LooseGameplayTag (RPG.State.Combat.Active)。
 *   Tag 切换驱动 GE_Stamina_Regen_OutOfCombat (Blocked) / GE_Stamina_Regen_InCombat (Required)
 *   两条 Periodic GE 的回耐力开关（参见 14 §3.4 资产 7/8）。
 *
 *   A3 阶段仅实现：
 *     - NotifyCombatAction(AActor*)         手动调用入口（A5 由武器 GA 命中/受击事件调）
 *     - IsInCombat(AActor*) const           查询接口（HUD/GE 用）
 *     - Tick                                每帧巡检超时（默认 2s 脱战）
 *     - Print Log                           验证链路
 *
 *   A5 阶段接入：URPGDamageExecution 命中后 → 主动调 NotifyCombatAction(Source) + NotifyCombatAction(Target)
 *
 *   详见 14_后续日程与验收清单.md §3.7 / 02 §F.10（5s 内造成/受到伤害判定为战斗中）
 */
UCLASS(MinimalAPI)
class URPGCombatStateSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:

	UE_API URPGCombatStateSubsystem();

	//~UWorldSubsystem
	UE_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UE_API virtual void Deinitialize() override;
	UE_API virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
	//~End

	//~FTickableGameObject
	UE_API virtual void Tick(float DeltaTime) override;
	UE_API virtual TStatId GetStatId() const override;
	UE_API virtual bool IsTickable() const override { return true; }
	UE_API virtual bool IsTickableInEditor() const override { return false; }
	UE_API virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	//~End

	/**
	 * 通知该 Actor 触发了战斗动作（命中、受击、释放敌对技能等）。
	 * 实现：刷新 LastCombatTimeMap[Actor] = Now，并立即给 ASC 加 LooseGameplayTag State.Combat.Active。
	 * A5 阶段由 URPGDamageExecution 的 Source/Target 双调用。
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Combat")
	UE_API void NotifyCombatAction(AActor* Actor);

	/** 查询 Actor 是否处于战斗状态（内部读 ASC.HasMatchingGameplayTag）。 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RPG|Combat")
	UE_API bool IsInCombat(AActor* Actor) const;

	/** 主动清除战斗状态（如玩家死亡复活、传送到主城）。 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Combat")
	UE_API void ClearCombatState(AActor* Actor);

	/** 脱战延迟（秒）。02 §F.10 锁定 5.0s（旧策划文档），A3 暂用 2.0s 与 §3.7 一致。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Combat", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float StaminaRegenDelay = 2.0f;

private:

	/** 取出 Actor 上的 ASC（PlayerState 或 Self）。 */
	static UAbilitySystemComponent* GetASC(AActor* Actor);

	/** 给 ASC 增/减 State.Combat.Active LooseGameplayTag（Idempotent）。 */
	static void SetCombatTag(UAbilitySystemComponent* ASC, bool bInCombat);

	/** Map<Actor, LastCombatTime>。WeakPtr 以便 Actor 销毁后自动清理（Tick 巡检时移除空键）。 */
	TMap<TWeakObjectPtr<AActor>, float> LastCombatTimeMap;
};

#undef UE_API
