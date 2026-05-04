#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "RPGDamageExecutionCalculation.generated.h"

/**
 * URPGDamageExecutionCalculation - 伤害计算执行类
 * 
 * 实现伤害计算的13个步骤：
 * 1. 生命 = 基础生命 * (1 + %生命) + 额外生命
 * 2. 攻击 = 基础攻击 * (1 + %攻击) + 额外攻击
 * 3. 防御 = 基础防御 * (1 + %防御) + 额外防御
 * 4. 暴击 = 基础暴击 + %暴击
 * 5. 暴伤 = 基础暴伤 + %暴伤
 * 6. 判定是否暴击
 * 7. 未减免伤害 = (基础 + Σ属性*加成) * (暴击结果加成)
 * 8. 承伤率 = 500 / (500 + 防御 * (1 - 防御穿透))
 * 9. 充能值 = 基础充能 * (1 + %充能倍率)
 * 10. 韧性减少值 = 基础破韧 * (1 + %击破加成)
 * 11. 最终伤害 = 未减免伤害 * 承伤率 * (1 + %增伤) * (1 - %减伤)
 * 12. 吸血量 = 实际伤害 * %吸血
 * 13. 血量回复 = 基础回复值 * (1 + 施加方的治疗效率)
 * 
 * 本类负责步骤1-11，步骤12-13由专门的ExecutionCalculation处理
 */
UCLASS()
class RPG_API URPGDamageExecutionCalculation : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    URPGDamageExecutionCalculation();

    /** 核心计算函数 - 实现伤害计算的13个步骤 */
    virtual void Execute_Implementation(
        const FGameplayEffectCustomExecutionParameters& ExecutionParams,
        FGameplayEffectCustomExecutionOutput& OutExecutionOutput
    ) const override;

protected:
    /** 
     * 步骤6：暴击判定
     * @param CritRate - 暴击率 (0.0 - 1.0)
     * @return true if 暴击触发
     */
    bool CheckCriticalHit(float CritRate) const;

    /** 
     * 步骤7：计算未减免伤害
     * @param BaseDamage - 基础伤害值
     * @param AttributeBonus - 属性加成总和
     * @param bIsCritical - 是否暴击
     * @param CritDmg - 暴伤加成
     * @return 未减免伤害
     */
    float CalculatePreMitigationDamage(float BaseDamage, float AttributeBonus, bool bIsCritical, float CritDmg) const;

    /** 
     * 步骤8：计算承伤率
     * @param Defense - 防御者的防御力
     * @param IgnoreDefense - 攻击者的防御穿透
     * @return 承伤率 (0.0 - 1.0)
     */
    float CalculateDamageReductionRate(float Defense, float IgnoreDefense) const;

    /** 
     * 步骤11：计算最终伤害
     * @param PreMitigationDamage - 未减免伤害
     * @param DamageReductionRate - 承伤率
     * @param DmgMul - 增伤百分比
     * @param DmgRedMul - 减伤百分比
     * @return 最终伤害
     */
    float CalculateFinalDamage(float PreMitigationDamage, float DamageReductionRate, float DmgMul, float DmgRedMul) const;
};
