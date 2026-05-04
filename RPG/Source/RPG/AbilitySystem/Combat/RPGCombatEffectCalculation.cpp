#include "RPGCombatEffectCalculation.h"
#include "../AttributeSystem/RPGAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "Engine/Engine.h"

// ============================================================
// URPGPowerRegenExecutionCalculation - 充能值计算
// ============================================================

URPGPowerRegenExecutionCalculation::URPGPowerRegenExecutionCalculation()
{
    // 定义需要读取的属性
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        URPGAttributeSet::GetPowerRegenRateAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true
    ));
}

void URPGPowerRegenExecutionCalculation::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    
    if (!SourceASC)
    {
        return;
    }

    const URPGAttributeSet* SourceAttrs = SourceASC->GetSet<URPGAttributeSet>();
    
    if (!SourceAttrs)
    {
        return;
    }

    // 步骤9：充能值 = 基础充能 * (1 + %充能倍率)
    float BasePower = 0.0f;
    
    FGameplayEffectAttributeCaptureDefinition PowerCaptureDef;
    PowerCaptureDef.AttributeToCapture = URPGAttributeSet::GetPowerRegenRateAttribute();
    PowerCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
    PowerCaptureDef.bSnapshot = false;
    
    FAggregatorEvaluateParameters EvalParams;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(PowerCaptureDef, EvalParams, BasePower);

    // 如果基础充能为0，使用默认值
    if (BasePower <= 0.0f)
    {
        BasePower = 10.0f; // 默认充能值
    }

    float PowerRegenRate = SourceAttrs->GetPowerRegenRate();
    float FinalPower = BasePower * (1.0f + PowerRegenRate);

    // 输出最终充能值
    // 注意：这里需要根据实际项目中的充能属性进行调整
    FGameplayModifierEvaluatedData PowerModifier;
    PowerModifier.Attribute = URPGAttributeSet::GetPowerRegenRateAttribute();
    PowerModifier.ModifierOp = EGameplayModOp::Additive;
    PowerModifier.Magnitude = FinalPower;
    OutExecutionOutput.AddOutputModifier(PowerModifier);

#if WITH_EDITOR
    if (GEngine)
    {
        FString DebugMsg = FString::Printf(TEXT("Power Regen: %.2f"), FinalPower);
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, DebugMsg);
    }
#endif
}

// ============================================================
// URPGBreakExecutionCalculation - 击破值计算
// ============================================================

URPGBreakExecutionCalculation::URPGBreakExecutionCalculation()
{
    // 定义需要读取的属性
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        URPGAttributeSet::GetBreakBonusAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true
    ));
}

void URPGBreakExecutionCalculation::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    
    if (!SourceASC)
    {
        return;
    }

    const URPGAttributeSet* SourceAttrs = SourceASC->GetSet<URPGAttributeSet>();
    
    if (!SourceAttrs)
    {
        return;
    }

    // 步骤10：韧性减少值 = 基础破韧 * (1 + %击破加成)
    float BaseBreak = 0.0f;
    
    FGameplayEffectAttributeCaptureDefinition BreakCaptureDef;
    BreakCaptureDef.AttributeToCapture = URPGAttributeSet::GetBreakBonusAttribute();
    BreakCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
    BreakCaptureDef.bSnapshot = false;
    
    FAggregatorEvaluateParameters EvalParams;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BreakCaptureDef, EvalParams, BaseBreak);

    // 如果基础破韧为0，使用默认值
    if (BaseBreak <= 0.0f)
    {
        BaseBreak = 10.0f; // 默认破韧值
    }

    float BreakBonus = SourceAttrs->GetBreakBonus();
    float FinalBreak = BaseBreak * (1.0f + BreakBonus);

    // 输出最终击破值
    FGameplayModifierEvaluatedData BreakModifier;
    BreakModifier.Attribute = URPGAttributeSet::GetBreakBonusAttribute();
    BreakModifier.ModifierOp = EGameplayModOp::Additive;
    BreakModifier.Magnitude = FinalBreak;
    OutExecutionOutput.AddOutputModifier(BreakModifier);

#if WITH_EDITOR
    if (GEngine)
    {
        FString DebugMsg = FString::Printf(TEXT("Break Value: %.2f"), FinalBreak);
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, DebugMsg);
    }
#endif
}

// ============================================================
// URPGHealExecutionCalculation - 治疗值计算
// ============================================================

URPGHealExecutionCalculation::URPGHealExecutionCalculation()
{
    // 定义需要读取的属性
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        URPGAttributeSet::GetHp_BasicAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true
    ));
}

void URPGHealExecutionCalculation::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    
    if (!SourceASC)
    {
        return;
    }

    const URPGAttributeSet* SourceAttrs = SourceASC->GetSet<URPGAttributeSet>();
    
    if (!SourceAttrs)
    {
        return;
    }

    // 步骤13：血量回复 = 基础回复值 * (1 + 施加方的治疗效率)
    float BaseHeal = 0.0f;
    
    FGameplayEffectAttributeCaptureDefinition HealCaptureDef;
    HealCaptureDef.AttributeToCapture = URPGAttributeSet::GetHp_BasicAttribute();
    HealCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
    HealCaptureDef.bSnapshot = false;
    
    FAggregatorEvaluateParameters EvalParams;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealCaptureDef, EvalParams, BaseHeal);

    // 如果基础回复值为0，使用默认值
    if (BaseHeal <= 0.0f)
    {
        BaseHeal = 10.0f; // 默认治疗值
    }

    // 注意：治疗效率属性需要在AttributeSet中添加
    // float HealEfficiency = SourceAttrs->GetHealEfficiency();
    // float FinalHeal = BaseHeal * (1.0f + HealEfficiency);
    float FinalHeal = BaseHeal; // 暂时不使用治疗效率

    // 输出最终治疗值（加到HP上）
    FGameplayModifierEvaluatedData HealModifier;
    HealModifier.Attribute = URPGAttributeSet::GetHp_BasicAttribute();
    HealModifier.ModifierOp = EGameplayModOp::Additive;
    HealModifier.Magnitude = FinalHeal;
    OutExecutionOutput.AddOutputModifier(HealModifier);

#if WITH_EDITOR
    if (GEngine)
    {
        FString DebugMsg = FString::Printf(TEXT("Heal: %.2f"), FinalHeal);
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, DebugMsg);
    }
#endif
}
