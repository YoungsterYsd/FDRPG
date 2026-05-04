#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "RPGCombatEffectCalculation.generated.h"

/**
 * URPGPowerRegenExecutionCalculation - 充能值计算
 * 
 * 步骤9：充能值 = 基础充能 * (1 + %充能倍率)
 * 
 * 使用方式：
 * 1. 创建GameplayEffect，添加此ExecutionCalculation
 * 2. 在GE的Modifier中设置基础充能值
 * 3. 此计算会自动应用%充能倍率加成
 */
UCLASS()
class RPG_API URPGPowerRegenExecutionCalculation : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    URPGPowerRegenExecutionCalculation();

    virtual void Execute_Implementation(
        const FGameplayEffectCustomExecutionParameters& ExecutionParams,
        FGameplayEffectCustomExecutionOutput& OutExecutionOutput
    ) const override;
};

/**
 * URPGBreakExecutionCalculation - 击破值计算
 * 
 * 步骤10：韧性减少值 = 基础破韧 * (1 + %击破加成)
 * 
 * 使用方式：
 * 1. 创建GameplayEffect，添加此ExecutionCalculation
 * 2. 在GE的Modifier中设置基础破韧值
 * 3. 此计算会自动应用%击破加成
 */
UCLASS()
class RPG_API URPGBreakExecutionCalculation : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    URPGBreakExecutionCalculation();

    virtual void Execute_Implementation(
        const FGameplayEffectCustomExecutionParameters& ExecutionParams,
        FGameplayEffectCustomExecutionOutput& OutExecutionOutput
    ) const override;
};

/**
 * URPGHealExecutionCalculation - 治疗值计算
 * 
 * 步骤13：血量回复 = 基础回复值 * (1 + 施加方的治疗效率)
 * 
 * 使用方式：
 * 1. 创建GameplayEffect，添加此ExecutionCalculation
 * 2. 在GE的Modifier中设置基础回复值
 * 3. 此计算会自动应用施加方的治疗效率
 * 
 * 注意：需要在AttributeSet中添加治疗效率属性
 */
UCLASS()
class RPG_API URPGHealExecutionCalculation : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    URPGHealExecutionCalculation();

    virtual void Execute_Implementation(
        const FGameplayEffectCustomExecutionParameters& ExecutionParams,
        FGameplayEffectCustomExecutionOutput& OutExecutionOutput
    ) const override;
};
