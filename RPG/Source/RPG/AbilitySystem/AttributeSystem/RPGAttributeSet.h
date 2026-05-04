#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RPGAttributeSet.generated.h"

// 使用这些宏来自动生成Getter和Setter函数
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * URPGAttributeSet - 属性系统核心类
 * 
 * 包含所有19个属性的定义，分为以下几类：
 * 1. 基础属性 (Hp_Basic, Atk_Basic, Def_Basic)
 * 2. 额外属性 (Hp_PostAdd, Atk_PostAdd, Def_PostAdd)
 * 3. 百分比属性 (Hp_Mul, Atk_Mul, Def_Mul)
 * 4. 战斗属性 (Crit, CritDmg, IgnDef, Dmg_Mul, DmgRed_Mul, HealthSteal)
 * 5. 特殊属性 (PowerRegenRate, BreakBonus, NormalSkillSpeed, MoveSpeed)
 * 6. 最终属性 (FinalHp, FinalAtk, FinalDef - 运行时计算，不存储)
 */
UCLASS()
class RPG_API URPGAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    URPGAttributeSet();

    // 重写GetLifetimeReplicatedProps用于网络复制
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // ============================================================
    // 1. 基础属性 (Basic)
    //    公式：最终属性 = 基础属性 * (1 + 百分比属性) + 额外属性
    // ============================================================

    /** 基础生命 - 生命值的基础数值 */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Hp_Basic, Category = "Vitality")
    FGameplayAttributeData Hp_Basic;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, Hp_Basic);

    /** 基础攻击 - 攻击力的基础数值 */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Atk_Basic, Category = "Attack")
    FGameplayAttributeData Atk_Basic;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, Atk_Basic);

    /** 基础防御 - 防御力的基础数值 */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Def_Basic, Category = "Defense")
    FGameplayAttributeData Def_Basic;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, Def_Basic);

    // ============================================================
    // 2. 额外属性 (PostAdd)
    //    直接加到最终属性上
    // ============================================================

    /** 额外生命 - 直接加成的生命值 */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Hp_PostAdd, Category = "Vitality")
    FGameplayAttributeData Hp_PostAdd;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, Hp_PostAdd);

    /** 额外攻击 - 直接加成的攻击力 */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Atk_PostAdd, Category = "Attack")
    FGameplayAttributeData Atk_PostAdd;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, Atk_PostAdd);

    /** 额外防御 - 直接加成的防御力 */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Def_PostAdd, Category = "Defense")
    FGameplayAttributeData Def_PostAdd;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, Def_PostAdd);

    // ============================================================
    // 3. 百分比属性 (Mul)
    //    以百分比形式加成基础属性
    // ============================================================

    /** %生命 - 生命值百分比加成 (0.0 - 1.0 表示 0% - 100%) */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Hp_Mul, Category = "Vitality")
    FGameplayAttributeData Hp_Mul;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, Hp_Mul);

    /** %攻击 - 攻击力百分比加成 (0.0 - 1.0 表示 0% - 100%) */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Atk_Mul, Category = "Attack")
    FGameplayAttributeData Atk_Mul;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, Atk_Mul);

    /** %防御 - 防御力百分比加成 (0.0 - 1.0 表示 0% - 100%) */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Def_Mul, Category = "Defense")
    FGameplayAttributeData Def_Mul;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, Def_Mul);

    // ============================================================
    // 4. 战斗属性 - 暴击
    // ============================================================

    /** %暴击 - 暴击率 (0.0 - 1.0 表示 0% - 100%) */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Crit, Category = "Combat|Critical")
    FGameplayAttributeData Crit;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, Crit);

    /** %暴伤 - 暴击伤害加成 (0.0 - 表示 0% 额外暴击伤害) */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritDmg, Category = "Combat|Critical")
    FGameplayAttributeData CritDmg;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, CritDmg);

    // ============================================================
    // 5. 战斗属性 - 防御穿透
    // ============================================================

    /** %防御穿透 - 忽略防御的百分比 (0.0 - 1.0 表示 0% - 100%) */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IgnDef, Category = "Combat|Defense")
    FGameplayAttributeData IgnDef;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, IgnDef);

    // ============================================================
    // 6. 战斗属性 - 充能
    // ============================================================

    /** %充能倍率 - 充能值加成 (0.0 - 表示 0% 额外充能) */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PowerRegenRate, Category = "Combat|Power")
    FGameplayAttributeData PowerRegenRate;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, PowerRegenRate);

    // ============================================================
    // 7. 战斗属性 - 击破
    // ============================================================

    /** %击破加成 - 击破值加成 (0.0 - 表示 0% 额外击破) */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BreakBonus, Category = "Combat|Break")
    FGameplayAttributeData BreakBonus;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, BreakBonus);

    // ============================================================
    // 8. 战斗属性 - 伤害修正
    // ============================================================

    /** %增伤 - 伤害加成 (0.0 - 表示 0% 额外伤害) */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Dmg_Mul, Category = "Combat|Damage")
    FGameplayAttributeData Dmg_Mul;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, Dmg_Mul);

    /** %减伤 - 伤害减免 (0.0 - 1.0 表示 0% - 100% 伤害减免) */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DmgRed_Mul, Category = "Combat|Damage")
    FGameplayAttributeData DmgRed_Mul;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, DmgRed_Mul);

    // ============================================================
    // 9. 战斗属性 - 吸血
    // ============================================================

    /** %吸血 - 伤害转化为生命值的比例 (0.0 - 1.0 表示 0% - 100%) */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthSteal, Category = "Combat|Lifesteal")
    FGameplayAttributeData HealthSteal;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, HealthSteal);

    // ============================================================
    // 10. 特殊属性 - 速度和移动
    // ============================================================

    /** %普攻速度 - 普通攻击速度加成 (0.0 - 表示 0% 额外攻速) */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_NormalSkillSpeed, Category = "Combat|Speed")
    FGameplayAttributeData NormalSkillSpeed;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, NormalSkillSpeed);

    /** 移动速度 - 角色移动速度 */
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeed, Category = "Movement")
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(URPGAttributeSet, MoveSpeed);

    // ============================================================
    // 最终属性计算函数 (不存储，实时计算)
    // ============================================================

    /** 计算最终生命值 = 基础生命 * (1 + %生命) + 额外生命 */
    UFUNCTION(BlueprintCallable, Category = "RPG|Attributes")
    float GetFinalHp() const;

    /** 计算最终攻击力 = 基础攻击 * (1 + %攻击) + 额外攻击 */
    UFUNCTION(BlueprintCallable, Category = "RPG|Attributes")
    float GetFinalAtk() const;

    /** 计算最终防御力 = 基础防御 * (1 + %防御) + 额外防御 */
    UFUNCTION(BlueprintCallable, Category = "RPG|Attributes")
    float GetFinalDef() const;

    /** 计算承伤率 = 500 / (500 + 防御 * (1 - 防御穿透)) */
    UFUNCTION(BlueprintCallable, Category = "RPG|Attributes")
    float GetDamageReductionRate() const;

protected:
    // 复制通知函数
    UFUNCTION()
    void OnRep_Hp_Basic(const FGameplayAttributeData& OldHp_Basic);

    UFUNCTION()
    void OnRep_Atk_Basic(const FGameplayAttributeData& OldAtk_Basic);

    UFUNCTION()
    void OnRep_Def_Basic(const FGameplayAttributeData& OldDef_Basic);

    UFUNCTION()
    void OnRep_Hp_PostAdd(const FGameplayAttributeData& OldHp_PostAdd);

    UFUNCTION()
    void OnRep_Atk_PostAdd(const FGameplayAttributeData& OldAtk_PostAdd);

    UFUNCTION()
    void OnRep_Def_PostAdd(const FGameplayAttributeData& OldDef_PostAdd);

    UFUNCTION()
    void OnRep_Hp_Mul(const FGameplayAttributeData& OldHp_Mul);

    UFUNCTION()
    void OnRep_Atk_Mul(const FGameplayAttributeData& OldAtk_Mul);

    UFUNCTION()
    void OnRep_Def_Mul(const FGameplayAttributeData& OldDef_Mul);

    UFUNCTION()
    void OnRep_Crit(const FGameplayAttributeData& OldCrit);

    UFUNCTION()
    void OnRep_CritDmg(const FGameplayAttributeData& OldCritDmg);

    UFUNCTION()
    void OnRep_IgnDef(const FGameplayAttributeData& OldIgnDef);

    UFUNCTION()
    void OnRep_PowerRegenRate(const FGameplayAttributeData& OldPowerRegenRate);

    UFUNCTION()
    void OnRep_BreakBonus(const FGameplayAttributeData& OldBreakBonus);

    UFUNCTION()
    void OnRep_Dmg_Mul(const FGameplayAttributeData& OldDmg_Mul);

    UFUNCTION()
    void OnRep_DmgRed_Mul(const FGameplayAttributeData& OldDmgRed_Mul);

    UFUNCTION()
    void OnRep_HealthSteal(const FGameplayAttributeData& OldHealthSteal);

    UFUNCTION()
    void OnRep_NormalSkillSpeed(const FGameplayAttributeData& OldNormalSkillSpeed);

    UFUNCTION()
    void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);
};
