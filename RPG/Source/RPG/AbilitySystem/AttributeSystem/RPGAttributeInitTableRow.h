#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RPGAttributeInitTableRow.generated.h"

/**
 * FRPGAttributeInitTableRow - 属性初始化表格行结构
 * 
 * 用于DataTable，从CSV导入
 * CSV列结构：ID,Name,Hp_Basic,Atk_Basic,Def_Basic,Hp_PostAdd,Atk_PostAdd,Def_PostAdd,Hp_Mul,Atk_Mul,Def_Mul,Crit,CritDmg,IgnDef,PowerRegenRate,BreakBonus,Dmg_Mul,DmgRed_Mul,HealthSteal,NormalSkillSpeed,MoveSpeed,Note
 * 
 * 属性初始化通过代码直接读取CSV数值，使用 SetNumericAttributeBase 设置到ASC，无需手动创建GE资产
 */
USTRUCT(BlueprintType)
struct FRPGAttributeInitTableRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    /** 名称 - 用于策划识别（Row Name即为Key，不需要额外字段） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit")
    FString Name;

    // ============================================================
    // 1. 基础属性 (Basic)
    // ============================================================
    
    /** 基础生命 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|Basic")
    float Hp_Basic;

    /** 基础攻击 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|Basic")
    float Atk_Basic;

    /** 基础防御 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|Basic")
    float Def_Basic;

    // ============================================================
    // 2. 额外属性 (PostAdd)
    // ============================================================
    
    /** 额外生命 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|PostAdd")
    float Hp_PostAdd;

    /** 额外攻击 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|PostAdd")
    float Atk_PostAdd;

    /** 额外防御 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|PostAdd")
    float Def_PostAdd;

    // ============================================================
    // 3. 百分比属性 (Mul)
    // ============================================================
    
    /** %生命 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|Mul")
    float Hp_Mul;

    /** %攻击 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|Mul")
    float Atk_Mul;

    /** %防御 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|Mul")
    float Def_Mul;

    // ============================================================
    // 4. 战斗属性 - 暴击
    // ============================================================
    
    /** %暴击 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|Combat")
    float Crit;

    /** %暴伤 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|Combat")
    float CritDmg;

    // ============================================================
    // 5. 战斗属性 - 防御穿透
    // ============================================================
    
    /** %防御穿透 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|Combat")
    float IgnDef;

    // ============================================================
    // 6. 战斗属性 - 充能
    // ============================================================
    
    /** %充能倍率 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|Combat")
    float PowerRegenRate;

    // ============================================================
    // 7. 战斗属性 - 击破
    // ============================================================
    
    /** %击破加成 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|Combat")
    float BreakBonus;

    // ============================================================
    // 8. 战斗属性 - 伤害修正
    // ============================================================
    
    /** %增伤 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|Combat")
    float Dmg_Mul;

    /** %减伤 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|Combat")
    float DmgRed_Mul;

    // ============================================================
    // 9. 战斗属性 - 吸血
    // ============================================================
    
    /** %吸血 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|Combat")
    float HealthSteal;

    // ============================================================
    // 10. 特殊属性 - 速度
    // ============================================================
    
    /** %普攻速度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|Speed")
    float NormalSkillSpeed;

    // ============================================================
    // 11. 特殊属性 - 移动
    // ============================================================
    
    /** 移动速度 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit|Movement")
    float MoveSpeed;

    // ============================================================
    // 12. 备注（不参与初始化，供策划识别）
    // ============================================================
    
    /** 备注 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttributeInit")
    FString Note;

public:
    /** 默认构造函数 - 初始化所有属性为0 */
    FRPGAttributeInitTableRow()
        : Hp_Basic(0.0f)
        , Atk_Basic(0.0f)
        , Def_Basic(0.0f)
        , Hp_PostAdd(0.0f)
        , Atk_PostAdd(0.0f)
        , Def_PostAdd(0.0f)
        , Hp_Mul(0.0f)
        , Atk_Mul(0.0f)
        , Def_Mul(0.0f)
        , Crit(0.0f)
        , CritDmg(0.0f)
        , IgnDef(0.0f)
        , PowerRegenRate(0.0f)
        , BreakBonus(0.0f)
        , Dmg_Mul(0.0f)
        , DmgRed_Mul(0.0f)
        , HealthSteal(0.0f)
        , NormalSkillSpeed(0.0f)
        , MoveSpeed(0.0f)
    {
    }
};
