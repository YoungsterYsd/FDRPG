#include "RPGDamageExecutionCalculation.h"
#include "../AttributeSystem/RPGAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "Engine/Engine.h"

URPGDamageExecutionCalculation::URPGDamageExecutionCalculation()
{
    // 定义需要读取的属性
    // 攻击者属性（Source）
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        URPGAttributeSet::GetAtk_BasicAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true
    ));
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        URPGAttributeSet::GetAtk_MulAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true
    ));
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        URPGAttributeSet::GetAtk_PostAddAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true
    ));
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        URPGAttributeSet::GetCritAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true
    ));
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        URPGAttributeSet::GetCritDmgAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true
    ));
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        URPGAttributeSet::GetDmg_MulAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true
    ));
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        URPGAttributeSet::GetIgnDefAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true
    ));
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        URPGAttributeSet::GetHealthStealAttribute(),
        EGameplayEffectAttributeCaptureSource::Source,
        true
    ));
    
    // 防御者属性（Target）
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        URPGAttributeSet::GetDef_BasicAttribute(),
        EGameplayEffectAttributeCaptureSource::Target,
        true
    ));
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        URPGAttributeSet::GetDef_MulAttribute(),
        EGameplayEffectAttributeCaptureSource::Target,
        true
    ));
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        URPGAttributeSet::GetDef_PostAddAttribute(),
        EGameplayEffectAttributeCaptureSource::Target,
        true
    ));
    RelevantAttributesToCapture.Add(FGameplayEffectAttributeCaptureDefinition(
        URPGAttributeSet::GetDmgRed_MulAttribute(),
        EGameplayEffectAttributeCaptureSource::Target,
        true
    ));
}

void URPGDamageExecutionCalculation::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    // ============================================================
    // 步骤1-3：获取攻击者和防御者的属性
    // ============================================================
    
    UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    if (!SourceASC || !TargetASC)
    {
        return;
    }

    // 获取Source和Target的AvatarActor
    AActor* SourceActor = SourceASC->GetAvatarActor();
    AActor* TargetActor = TargetASC->GetAvatarActor();

    // 获取AttributeSets
    const URPGAttributeSet* SourceAttrs = SourceASC->GetSet<URPGAttributeSet>();
    const URPGAttributeSet* TargetAttrs = TargetASC->GetSet<URPGAttributeSet>();

    if (!SourceAttrs || !TargetAttrs)
    {
        return;
    }

    // ============================================================
    // 步骤1-3：计算最终攻击力和防御力
    // ============================================================
    
    float FinalAtk = SourceAttrs->GetFinalAtk();  // 攻击 = 基础攻击 * (1 + %攻击) + 额外攻击
    float FinalDef = TargetAttrs->GetFinalDef();   // 防御 = 基础防御 * (1 + %防御) + 额外防御

    // ============================================================
    // 步骤4-5：获取暴击率和暴击伤害
    // ============================================================
    
    float CritRate = FMath::Clamp(SourceAttrs->GetCrit(), 0.0f, 1.0f);
    float CritDmg = SourceAttrs->GetCritDmg();

    // ============================================================
    // 步骤6：判定是否暴击
    // ============================================================
    
    bool bIsCritical = CheckCriticalHit(CritRate);
    float CritMultiplier = bIsCritical ? (1.0f + CritDmg) : 1.0f;

    // ============================================================
    // 步骤7：计算未减免伤害
    // ============================================================
    
    // 从GameplayEffectSpec中获取基础伤害
    float BaseDamage = 0.0f;
    
    // 创建CaptureDefinition用于读取属性
    FGameplayEffectAttributeCaptureDefinition AtkCaptureDef;
    AtkCaptureDef.AttributeToCapture = URPGAttributeSet::GetAtk_BasicAttribute();
    AtkCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
    AtkCaptureDef.bSnapshot = false;
    
    FAggregatorEvaluateParameters EvalParams;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(AtkCaptureDef, EvalParams, BaseDamage);

    // 如果基础伤害为0，使用最终攻击力作为基础伤害
    if (BaseDamage <= 0.0f)
    {
        BaseDamage = FinalAtk;
    }

    // 计算属性加成（可以从GE的Modifier中读取）
    float AttributeBonus = 0.0f;
    // TODO: 可以从ExecutionParams中获取额外的属性加成

    float PreMitigationDamage = CalculatePreMitigationDamage(BaseDamage, AttributeBonus, bIsCritical, CritDmg);

    // ============================================================
    // 步骤8：计算承伤率
    // ============================================================
    
    float IgnoreDefense = SourceAttrs->GetIgnDef();
    float DamageReductionRate = CalculateDamageReductionRate(FinalDef, IgnoreDefense);

    // ============================================================
    // 步骤9-10：计算充能值和击破值（由其他Execution处理）
    // ============================================================
    // 这些步骤在专门的ExecutionCalculation中处理

    // ============================================================
    // 步骤11：计算最终伤害
    // ============================================================
    
    float DmgMul = SourceAttrs->GetDmg_Mul();
    float DmgRedMul = TargetAttrs->GetDmgRed_Mul();
    float FinalDamage = CalculateFinalDamage(PreMitigationDamage, DamageReductionRate, DmgMul, DmgRedMul);

    // 确保伤害不为负数
    FinalDamage = FMath::Max(0.0f, FinalDamage);

    // ============================================================
    // 输出最终伤害
    // ============================================================
    
    FGameplayModifierEvaluatedData DamageModifier;
    DamageModifier.Attribute = URPGAttributeSet::GetHp_BasicAttribute();
    DamageModifier.ModifierOp = EGameplayModOp::Additive;
    DamageModifier.Magnitude = -FinalDamage;
    OutExecutionOutput.AddOutputModifier(DamageModifier);

    // ============================================================
    // 步骤12：计算吸血量（输出到OutExecutionOutput）
    // ============================================================
    
    float HealthStealRate = SourceAttrs->GetHealthSteal();
    if (HealthStealRate > 0.0f)
    {
        float HealthStealAmount = FinalDamage * HealthStealRate;
        
        // 输出吸血量（通过HealthSteal属性传递）
        FGameplayModifierEvaluatedData HealModifier;
        HealModifier.Attribute = URPGAttributeSet::GetHp_BasicAttribute();
        HealModifier.ModifierOp = EGameplayModOp::Additive;
        HealModifier.Magnitude = HealthStealAmount;
        OutExecutionOutput.AddOutputModifier(HealModifier);
    }

    // 可以在这里输出调试信息
#if WITH_EDITOR
    if (GEngine)
    {
        FString DebugMsg = FString::Printf(TEXT("Damage: %.2f (Crit: %s)"), FinalDamage, bIsCritical ? TEXT("Yes") : TEXT("No"));
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, DebugMsg);
    }
#endif
}

bool URPGDamageExecutionCalculation::CheckCriticalHit(float CritRate) const
{
    // 暴击判定：随机数 < 暴击率 则暴击
    float RandomValue = FMath::FRand();
    return RandomValue < CritRate;
}

float URPGDamageExecutionCalculation::CalculatePreMitigationDamage(float BaseDamage, float AttributeBonus, bool bIsCritical, float CritDmg) const
{
    // 未减免伤害 = (基础 + Σ属性*加成) * (暴击结果加成)
    float CritMultiplier = bIsCritical ? (1.0f + CritDmg) : 1.0f;
    float PreMitigationDamage = (BaseDamage + AttributeBonus) * CritMultiplier;
    
    return PreMitigationDamage;
}

float URPGDamageExecutionCalculation::CalculateDamageReductionRate(float Defense, float IgnoreDefense) const
{
    // 承伤率 = 500 / (500 + 防御 * (1 - 防御穿透))
    float ClampedIgnoreDef = FMath::Clamp(IgnoreDefense, 0.0f, 1.0f);
    float EffectiveDefense = Defense * (1.0f - ClampedIgnoreDef);
    float ReductionRate = 500.0f / (500.0f + EffectiveDefense);
    
    return ReductionRate;
}

float URPGDamageExecutionCalculation::CalculateFinalDamage(float PreMitigationDamage, float DamageReductionRate, float DmgMul, float DmgRedMul) const
{
    // 最终伤害 = 未减免伤害 * 承伤率 * (1 + %增伤) * (1 - %减伤)
    float FinalDamage = PreMitigationDamage * DamageReductionRate * (1.0f + DmgMul) * (1.0f - DmgRedMul);
    
    return FinalDamage;
}
