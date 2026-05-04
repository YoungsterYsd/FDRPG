#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RPGCombatLibrary.generated.h"

/**
 * URPGCombatLibrary - 战斗函数库
 * 
 * 提供蓝图中常用的战斗计算函数
 * 封装属性系统和伤害计算的常用操作
 */
UCLASS()
class RPG_API URPGCombatLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** 
     * 暴击判定
     * @param CritRate - 暴击率 (0.0 - 1.0)
     * @return true if 暴击触发
     */
    UFUNCTION(BlueprintPure, Category = "RPG|Combat", meta = (DisplayName = "Check Critical Hit"))
    static bool CheckCriticalHit(float CritRate);

    /** 
     * 计算承伤率
     * @param Defense - 防御力
     * @param IgnoreDefense - 防御穿透 (0.0 - 1.0)
     * @return 承伤率 (0.0 - 1.0)
     */
    UFUNCTION(BlueprintPure, Category = "RPG|Combat", meta = (DisplayName = "Calculate Damage Reduction Rate"))
    static float CalculateDamageReductionRate(float Defense, float IgnoreDefense);

    /** 
     * 计算最终伤害
     * @param BaseDamage - 基础伤害
     * @param DamageMultiplier - 伤害倍率（包含暴击加成）
     * @param DamageReductionRate - 承伤率
     * @param DmgMul - 增伤百分比
     * @param DmgRedMul - 减伤百分比
     * @return 最终伤害
     */
    UFUNCTION(BlueprintPure, Category = "RPG|Combat", meta = (DisplayName = "Calculate Final Damage"))
    static float CalculateFinalDamage(float BaseDamage, float DamageMultiplier, float DamageReductionRate, float DmgMul, float DmgRedMul);

    /** 
     * 计算吸血量
     * @param ActualDamage - 实际伤害
     * @param HealthStealRate - 吸血率 (0.0 - 1.0)
     * @return 吸血量
     */
    UFUNCTION(BlueprintPure, Category = "RPG|Combat", meta = (DisplayName = "Calculate Health Steal"))
    static float CalculateHealthSteal(float ActualDamage, float HealthStealRate);

    /** 
     * 计算最终属性值
     * @param Basic - 基础值
     * @param Mul - 百分比加成
     * @param PostAdd - 额外加成
     * @return 最终值
     */
    UFUNCTION(BlueprintPure, Category = "RPG|Combat", meta = (DisplayName = "Calculate Final Attribute"))
    static float CalculateFinalAttribute(float Basic, float Mul, float PostAdd);

    /** 
     * 计算充能值
     * @param BasePower - 基础充能
     * @param PowerRegenRate - 充能倍率
     * @return 最终充能值
     */
    UFUNCTION(BlueprintPure, Category = "RPG|Combat", meta = (DisplayName = "Calculate Power Regen"))
    static float CalculatePowerRegen(float BasePower, float PowerRegenRate);

    /** 
     * 计算击破值
     * @param BaseBreak - 基础破韧
     * @param BreakBonus - 击破加成
     * @return 最终击破值
     */
    UFUNCTION(BlueprintPure, Category = "RPG|Combat", meta = (DisplayName = "Calculate Break Value"))
    static float CalculateBreakValue(float BaseBreak, float BreakBonus);

    /** 
     * 计算治疗值
     * @param BaseHeal - 基础治疗
     * @param HealEfficiency - 治疗效率
     * @return 最终治疗值
     */
    UFUNCTION(BlueprintPure, Category = "RPG|Combat", meta = (DisplayName = "Calculate Heal Value"))
    static float CalculateHealValue(float BaseHeal, float HealEfficiency);

    /**
     * 从Actor获取RPGAttributeSet
     * @param Actor - 目标Actor
     * @return RPGAttributeSet指针，如果不存在则返回nullptr
     */
    UFUNCTION(BlueprintPure, Category = "RPG|Combat", meta = (DisplayName = "Get RPG Attribute Set"))
    static const class URPGAttributeSet* GetRPGAttributeSet(AActor* Actor);
};
