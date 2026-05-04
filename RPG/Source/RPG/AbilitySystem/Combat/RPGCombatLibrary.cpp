#include "RPGCombatLibrary.h"
#include "../AttributeSystem/RPGAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"

bool URPGCombatLibrary::CheckCriticalHit(float CritRate)
{
    // 暴击判定：随机数 < 暴击率 则暴击
    float RandomValue = FMath::FRand();
    return RandomValue < FMath::Clamp(CritRate, 0.0f, 1.0f);
}

float URPGCombatLibrary::CalculateDamageReductionRate(float Defense, float IgnoreDefense)
{
    // 承伤率 = 500 / (500 + 防御 * (1 - 防御穿透))
    float ClampedIgnoreDef = FMath::Clamp(IgnoreDefense, 0.0f, 1.0f);
    float EffectiveDefense = Defense * (1.0f - ClampedIgnoreDef);
    float ReductionRate = 500.0f / (500.0f + EffectiveDefense);
    
    return ReductionRate;
}

float URPGCombatLibrary::CalculateFinalDamage(float BaseDamage, float DamageMultiplier, float DamageReductionRate, float DmgMul, float DmgRedMul)
{
    // 最终伤害 = 基础伤害 * 伤害倍率 * 承伤率 * (1 + %增伤) * (1 - %减伤)
    float FinalDamage = BaseDamage * DamageMultiplier * DamageReductionRate * (1.0f + DmgMul) * (1.0f - DmgRedMul);
    
    return FMath::Max(0.0f, FinalDamage);
}

float URPGCombatLibrary::CalculateHealthSteal(float ActualDamage, float HealthStealRate)
{
    // 吸血量 = 实际伤害 * %吸血
    return ActualDamage * FMath::Clamp(HealthStealRate, 0.0f, 1.0f);
}

float URPGCombatLibrary::CalculateFinalAttribute(float Basic, float Mul, float PostAdd)
{
    // 最终属性 = 基础属性 * (1 + %属性) + 额外属性
    return Basic * (1.0f + Mul) + PostAdd;
}

float URPGCombatLibrary::CalculatePowerRegen(float BasePower, float PowerRegenRate)
{
    // 充能值 = 基础充能 * (1 + %充能倍率)
    return BasePower * (1.0f + PowerRegenRate);
}

float URPGCombatLibrary::CalculateBreakValue(float BaseBreak, float BreakBonus)
{
    // 击破值 = 基础破韧 * (1 + %击破加成)
    return BaseBreak * (1.0f + BreakBonus);
}

float URPGCombatLibrary::CalculateHealValue(float BaseHeal, float HealEfficiency)
{
    // 血量回复 = 基础回复值 * (1 + 施加方的治疗效率)
    return BaseHeal * (1.0f + HealEfficiency);
}

const URPGAttributeSet* URPGCombatLibrary::GetRPGAttributeSet(AActor* Actor)
{
    if (!Actor)
    {
        return nullptr;
    }

    // 使用AbilitySystemBlueprintLibrary获取ASC
    UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
    
    if (ASC)
    {
        return ASC->GetSet<URPGAttributeSet>();
    }

    return nullptr;
}
