// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGPrimaryAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGPrimaryAttributeSet)

URPGPrimaryAttributeSet::URPGPrimaryAttributeSet()
{
	// v7 锁定：C++ 不硬编码任何初值。
	// 所有字段默认 0.0f（FGameplayAttributeData 默认构造），
	// 由 GE_PrimaryAttributes_Init (Instant) Override 赋予 19 个初值，
	// AttackFinal/DefenseFinal 由对应 Derive GE 持续派生。
	// 详见 14_后续日程与验收清单.md §3.3.2 / §3.3.4
}

void URPGPrimaryAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 组 1：基础三维
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, HealthBase, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, AttackBase, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, DefenseBase, COND_None, REPNOTIFY_Always);

	// 组 2：加性加成
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, HealthBonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, AttackBonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, DefenseBonus, COND_None, REPNOTIFY_Always);

	// 组 3：百分比乘数
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, HealthMul, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, AttackMul, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, DefenseMul, COND_None, REPNOTIFY_Always);

	// 组 4：汇总派生
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, AttackFinal, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, DefenseFinal, COND_None, REPNOTIFY_Always);

	// 组 5：战斗倍率
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, CritChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, CritDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, DefensePenetration, COND_None, REPNOTIFY_Always);

	// 组 6：伤害修正
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, DamageBonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, DamageReduction, COND_None, REPNOTIFY_Always);

	// 组 7：特殊
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, LifeSteal, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, EnergyGainMul, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, BreakBonus, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGPrimaryAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void URPGPrimaryAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// 钳制：基础三维不允许 < 0；战斗倍率/伤害修正不允许 < 0；CritChance 限 [0, 1]
	if (Attribute == GetHealthBaseAttribute() ||
		Attribute == GetAttackBaseAttribute() ||
		Attribute == GetDefenseBaseAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	else if (Attribute == GetCritChanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 1.0f);
	}
	else if (Attribute == GetDefensePenetrationAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 1.0f);
	}
	else if (Attribute == GetDamageReductionAttribute())
	{
		// 减伤不能 ≥ 1（避免完全免疫导致伤害公式除零或负数）
		NewValue = FMath::Clamp(NewValue, 0.0f, 0.95f);
	}
	else if (Attribute == GetLifeStealAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 1.0f);
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	// AttackFinal / DefenseFinal 由 GE Override 写入，不在此钳制（GE 公式保证非负）
	// HealthBonus/Mul、Damage* 等可正可负（Debuff 用），不钳制
}

// === OnRep 回调（仅 REPNOTIFY，无业务逻辑）===
#define DEFINE_RPG_PRIMARY_ATTR_ONREP(AttrName) \
	void URPGPrimaryAttributeSet::OnRep_##AttrName(const FGameplayAttributeData& OldValue) \
	{ \
		GAMEPLAYATTRIBUTE_REPNOTIFY(URPGPrimaryAttributeSet, AttrName, OldValue); \
	}

DEFINE_RPG_PRIMARY_ATTR_ONREP(HealthBase)
DEFINE_RPG_PRIMARY_ATTR_ONREP(AttackBase)
DEFINE_RPG_PRIMARY_ATTR_ONREP(DefenseBase)
DEFINE_RPG_PRIMARY_ATTR_ONREP(HealthBonus)
DEFINE_RPG_PRIMARY_ATTR_ONREP(AttackBonus)
DEFINE_RPG_PRIMARY_ATTR_ONREP(DefenseBonus)
DEFINE_RPG_PRIMARY_ATTR_ONREP(HealthMul)
DEFINE_RPG_PRIMARY_ATTR_ONREP(AttackMul)
DEFINE_RPG_PRIMARY_ATTR_ONREP(DefenseMul)
DEFINE_RPG_PRIMARY_ATTR_ONREP(AttackFinal)
DEFINE_RPG_PRIMARY_ATTR_ONREP(DefenseFinal)
DEFINE_RPG_PRIMARY_ATTR_ONREP(CritChance)
DEFINE_RPG_PRIMARY_ATTR_ONREP(CritDamage)
DEFINE_RPG_PRIMARY_ATTR_ONREP(DefensePenetration)
DEFINE_RPG_PRIMARY_ATTR_ONREP(DamageBonus)
DEFINE_RPG_PRIMARY_ATTR_ONREP(DamageReduction)
DEFINE_RPG_PRIMARY_ATTR_ONREP(LifeSteal)
DEFINE_RPG_PRIMARY_ATTR_ONREP(EnergyGainMul)
DEFINE_RPG_PRIMARY_ATTR_ONREP(BreakBonus)
DEFINE_RPG_PRIMARY_ATTR_ONREP(AttackSpeed)
DEFINE_RPG_PRIMARY_ATTR_ONREP(MoveSpeed)

#undef DEFINE_RPG_PRIMARY_ATTR_ONREP
