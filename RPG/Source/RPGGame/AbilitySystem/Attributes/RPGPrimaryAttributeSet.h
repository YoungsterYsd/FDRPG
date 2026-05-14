// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "RPGAttributeSet.h"

#include "RPGPrimaryAttributeSet.generated.h"

#define UE_API RPGGAME_API

class UObject;
struct FFrame;
struct FGameplayEffectModCallbackData;

/**
 * URPGPrimaryAttributeSet
 *
 * RPG 主属性集（A3 v7 锁定）：21 字段，按旧案 `角色系统_属性_旧案.md` 重做，分 7 组。
 * 命名规范：[目标在前，修饰在后]，全部 PascalCase + 后缀（Base/Bonus/Mul/Final）。
 * 初值：C++ 不硬编码，全部由 GE_PrimaryAttributes_Init Override 赋予（详见 14 文档 §3.3）。
 *
 * 组 1：基础三维（*Base）        Health/Attack/Defense Base
 * 组 2：加性加成（*Bonus）       装备/词条加性叠加
 * 组 3：百分比乘数（*Mul）       Boon/词条乘性叠加
 * 组 4：汇总派生（*Final）       AttackFinal/DefenseFinal（GE 派生，仅读，面板用）
 *                                 MaxHealth 派生写入 HealthSet（不在本集）
 * 组 5：战斗倍率                 CritChance/CritDamage/DefensePenetration
 * 组 6：伤害修正                 DamageBonus/DamageReduction
 * 组 7：特殊                     LifeSteal/EnergyGainMul/BreakBonus/AttackSpeed/MoveSpeed
 */
UCLASS(MinimalAPI, BlueprintType)
class URPGPrimaryAttributeSet : public URPGAttributeSet
{
	GENERATED_BODY()

public:

	UE_API URPGPrimaryAttributeSet();

	// === 组 1：基础三维（*Base）===
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, HealthBase);
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, AttackBase);
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, DefenseBase);

	// === 组 2：加性加成（*Bonus）===
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, HealthBonus);
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, AttackBonus);
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, DefenseBonus);

	// === 组 3：百分比乘数（*Mul）===
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, HealthMul);
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, AttackMul);
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, DefenseMul);

	// === 组 4：汇总派生（*Final，GE Override 写入，仅读）===
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, AttackFinal);
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, DefenseFinal);

	// === 组 5：战斗倍率 ===
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, CritChance);
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, CritDamage);
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, DefensePenetration);

	// === 组 6：伤害修正 ===
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, DamageBonus);
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, DamageReduction);

	// === 组 7：特殊 ===
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, LifeSteal);
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, EnergyGainMul);
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, BreakBonus);
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, AttackSpeed);
	ATTRIBUTE_ACCESSORS(URPGPrimaryAttributeSet, MoveSpeed);

protected:

	UE_API virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UE_API virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	// OnRep 回调（Replicated 字段必备）
	UFUNCTION() UE_API void OnRep_HealthBase(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_AttackBase(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_DefenseBase(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_HealthBonus(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_AttackBonus(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_DefenseBonus(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_HealthMul(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_AttackMul(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_DefenseMul(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_AttackFinal(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_DefenseFinal(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_CritChance(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_CritDamage(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_DefensePenetration(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_DamageBonus(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_DamageReduction(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_LifeSteal(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_EnergyGainMul(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_BreakBonus(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_AttackSpeed(const FGameplayAttributeData& OldValue);
	UFUNCTION() UE_API void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

private:

	// === 组 1：基础三维（*Base）===
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthBase, Category = "RPG|Attribute|Base", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HealthBase;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackBase, Category = "RPG|Attribute|Base", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackBase;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DefenseBase, Category = "RPG|Attribute|Base", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DefenseBase;

	// === 组 2：加性加成（*Bonus）===
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthBonus, Category = "RPG|Attribute|Bonus", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HealthBonus;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackBonus, Category = "RPG|Attribute|Bonus", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackBonus;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DefenseBonus, Category = "RPG|Attribute|Bonus", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DefenseBonus;

	// === 组 3：百分比乘数（*Mul）===
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthMul, Category = "RPG|Attribute|Mul", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HealthMul;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackMul, Category = "RPG|Attribute|Mul", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackMul;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DefenseMul, Category = "RPG|Attribute|Mul", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DefenseMul;

	// === 组 4：汇总派生（*Final，由 GE_Attack_Derive_Final / GE_Defense_Derive_Final 通过 MMC Override 写入）===
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackFinal, Category = "RPG|Attribute|Final", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackFinal;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DefenseFinal, Category = "RPG|Attribute|Final", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DefenseFinal;

	// === 组 5：战斗倍率 ===
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritChance, Category = "RPG|Attribute|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CritChance;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritDamage, Category = "RPG|Attribute|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CritDamage;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DefensePenetration, Category = "RPG|Attribute|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DefensePenetration;

	// === 组 6：伤害修正 ===
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageBonus, Category = "RPG|Attribute|DamageMod", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DamageBonus;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageReduction, Category = "RPG|Attribute|DamageMod", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DamageReduction;

	// === 组 7：特殊 ===
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LifeSteal, Category = "RPG|Attribute|Special", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData LifeSteal;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_EnergyGainMul, Category = "RPG|Attribute|Special", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData EnergyGainMul;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BreakBonus, Category = "RPG|Attribute|Special", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BreakBonus;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackSpeed, Category = "RPG|Attribute|Special", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackSpeed;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeed, Category = "RPG|Attribute|Special", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MoveSpeed;
};

#undef UE_API
