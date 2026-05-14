// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "RPGAttributeSet.h"
#include "NativeGameplayTags.h"

#include "RPGHealthSet.generated.h"

#define UE_API RPGGAME_API

class UObject;
struct FFrame;

// === Lyra 沿用：通用 Damage 流程 Tag（仍保留，DamageExecution 引用）===
RPGGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Damage);
RPGGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity);
RPGGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageSelfDestruct);
RPGGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_FellOutOfWorld);
RPGGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_RPG_Damage_Message);

struct FGameplayEffectModCallbackData;

/**
 * URPGHealthSet（A3 v7 锁定 · 6 字段瘦身版）
 *
 *   设计原则：AttributeSet = 纯数值状态 + Meta 通道。
 *   字段命名：v7 后缀制，彻底废弃 Lyra 原生 Health/MaxHealth/Healing/Damage 命名。
 *
 *   字段：
 *     - HealthFinal      当前血量值（钳制 [0, HealthMax]）
 *     - HealthMax        最大血量（由 GE_Health_Derive_Max 持续派生写入）
 *     - HealthHealing    Meta 通道：流入血量的治疗值（PostExecute 转 HealthFinal+ 后清零）
 *     - HealthDamage     Meta 通道：流入血量的伤害值（PostExecute 转 HealthFinal- 后清零）
 *     - StaminaCurrent   当前耐力（格挡耐久；钳制 [0, StaminaMax]；= 0 广播 Block.Broken）
 *     - StaminaMax       最大耐力
 *
 *   初值：C++ 不硬编码，全部由 GE_HealthSet_Init / GE_Health_Derive_Max / GE_Health_Init_Full 赋予。
 *
 *   完整设计见 14_后续日程与验收清单.md §3.2.1 / §3.3
 */
UCLASS(MinimalAPI, BlueprintType)
class URPGHealthSet : public URPGAttributeSet
{
	GENERATED_BODY()

public:

	UE_API URPGHealthSet();

	ATTRIBUTE_ACCESSORS(URPGHealthSet, HealthFinal);
	ATTRIBUTE_ACCESSORS(URPGHealthSet, HealthMax);
	ATTRIBUTE_ACCESSORS(URPGHealthSet, HealthHealing);
	ATTRIBUTE_ACCESSORS(URPGHealthSet, HealthDamage);
	ATTRIBUTE_ACCESSORS(URPGHealthSet, StaminaCurrent);
	ATTRIBUTE_ACCESSORS(URPGHealthSet, StaminaMax);

	// Delegate when HealthFinal changes due to damage/healing, some information may be missing on the client
	mutable FRPGAttributeEvent OnHealthChanged;

	// Delegate when HealthMax changes
	mutable FRPGAttributeEvent OnMaxHealthChanged;

	// Delegate to broadcast when the HealthFinal attribute reaches zero
	mutable FRPGAttributeEvent OnOutOfHealth;

	// Delegate when StaminaCurrent reaches 0 (block broken)
	mutable FRPGAttributeEvent OnBlockBroken;

protected:

	UFUNCTION() UE_API void OnRep_HealthFinal(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_HealthMax(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_StaminaCurrent(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_StaminaMax(const FGameplayAttributeData& OldValue);

	UE_API virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UE_API virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	UE_API virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UE_API virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	UE_API virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	UE_API virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	UE_API void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	// === 当前血量（PostExecute 钳制 [0, HealthMax]；GE_Health_Init_Full 可 Override 写入）===
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthFinal, Category = "RPG|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HealthFinal;

	// === 最大血量（GE Override 写入）===
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthMax, Category = "RPG|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HealthMax;

	// === 当前耐力（格挡耐久；PostExecute 钳制 [0, StaminaMax]；GE_HealthSet_Init Override + Regen GE Add）===
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StaminaCurrent, Category = "RPG|Stamina", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData StaminaCurrent;

	// === 最大耐力 ===
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StaminaMax, Category = "RPG|Stamina", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData StaminaMax;

	// === 状态追踪 ===
	bool bOutOfHealth = false;
	bool bBlockBroken = false;

	// 用于在 PostExecute 中对比变化前后的值
	float HealthBeforeAttributeChange = 0.0f;
	float HealthMaxBeforeAttributeChange = 0.0f;
	float StaminaBeforeAttributeChange = 0.0f;

	// =========================================================================
	// Meta Attributes（事件通道，PostExecute 后立即清零）
	// =========================================================================

	// 流入治疗（→ +HealthFinal）
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HealthHealing;

	// 流入伤害（→ -HealthFinal）
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData HealthDamage;
};

#undef UE_API
