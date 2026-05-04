#include "RPGAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// 这个文件在 AttributeSystem/ 子文件夹中，include 路径不需要改变
// 因为所有 include 都是 "RPGAttributeSet.h" 这种同一文件夹的引用

URPGAttributeSet::URPGAttributeSet()
{
    // ============================================================
    // 初始化所有属性为0
    // ============================================================

    // 基础属性
    InitHp_Basic(100.0f);
    InitAtk_Basic(10.0f);
    InitDef_Basic(5.0f);

    // 额外属性
    InitHp_PostAdd(0.0f);
    InitAtk_PostAdd(0.0f);
    InitDef_PostAdd(0.0f);

    // 百分比属性 (0.0 = 0%)
    InitHp_Mul(0.0f);
    InitAtk_Mul(0.0f);
    InitDef_Mul(0.0f);

    // 战斗属性 - 暴击
    InitCrit(0.05f);      // 5% 基础暴击率
    InitCritDmg(0.5f);    // 50% 额外暴击伤害

    // 战斗属性 - 防御穿透
    InitIgnDef(0.0f);     // 0% 基础防御穿透

    // 战斗属性 - 充能
    InitPowerRegenRate(0.0f); // 0% 额外充能

    // 战斗属性 - 击破
    InitBreakBonus(0.0f);  // 0% 额外击破

    // 战斗属性 - 伤害修正
    InitDmg_Mul(0.0f);    // 0% 额外增伤
    InitDmgRed_Mul(0.0f); // 0% 基础减伤

    // 战斗属性 - 吸血
    InitHealthSteal(0.0f); // 0% 基础吸血

    // 特殊属性
    InitNormalSkillSpeed(0.0f); // 0% 额外攻速
    InitMoveSpeed(600.0f);      // 默认移动速度 600
}

void URPGAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 基础属性
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Hp_Basic, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Atk_Basic, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Def_Basic, COND_None, REPNOTIFY_Always);

    // 额外属性
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Hp_PostAdd, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Atk_PostAdd, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Def_PostAdd, COND_None, REPNOTIFY_Always);

    // 百分比属性
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Hp_Mul, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Atk_Mul, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Def_Mul, COND_None, REPNOTIFY_Always);

    // 战斗属性 - 暴击
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Crit, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, CritDmg, COND_None, REPNOTIFY_Always);

    // 战斗属性 - 防御穿透
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, IgnDef, COND_None, REPNOTIFY_Always);

    // 战斗属性 - 充能
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, PowerRegenRate, COND_None, REPNOTIFY_Always);

    // 战斗属性 - 击破
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, BreakBonus, COND_None, REPNOTIFY_Always);

    // 战斗属性 - 伤害修正
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Dmg_Mul, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, DmgRed_Mul, COND_None, REPNOTIFY_Always);

    // 战斗属性 - 吸血
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, HealthSteal, COND_None, REPNOTIFY_Always);

    // 特殊属性
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, NormalSkillSpeed, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}

// ============================================================
// 最终属性计算函数
// ============================================================

float URPGAttributeSet::GetFinalHp() const
{
    return GetHp_Basic() * (1.0f + GetHp_Mul()) + GetHp_PostAdd();
}

float URPGAttributeSet::GetFinalAtk() const
{
    return GetAtk_Basic() * (1.0f + GetAtk_Mul()) + GetAtk_PostAdd();
}

float URPGAttributeSet::GetFinalDef() const
{
    return GetDef_Basic() * (1.0f + GetDef_Mul()) + GetDef_PostAdd();
}

float URPGAttributeSet::GetDamageReductionRate() const
{
    float Defense = GetFinalDef();
    float IgnoreDefense = FMath::Clamp(GetIgnDef(), 0.0f, 1.0f);
    float EffectiveDefense = Defense * (1.0f - IgnoreDefense);
    
    // 承伤率 = 500 / (500 + 防御 * (1 - 防御穿透))
    return 500.0f / (500.0f + EffectiveDefense);
}

// ============================================================
// 复制通知函数 - 用于属性变化时的逻辑处理
// ============================================================

void URPGAttributeSet::OnRep_Hp_Basic(const FGameplayAttributeData& OldHp_Basic)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Hp_Basic, OldHp_Basic);
}

void URPGAttributeSet::OnRep_Atk_Basic(const FGameplayAttributeData& OldAtk_Basic)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Atk_Basic, OldAtk_Basic);
}

void URPGAttributeSet::OnRep_Def_Basic(const FGameplayAttributeData& OldDef_Basic)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Def_Basic, OldDef_Basic);
}

void URPGAttributeSet::OnRep_Hp_PostAdd(const FGameplayAttributeData& OldHp_PostAdd)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Hp_PostAdd, OldHp_PostAdd);
}

void URPGAttributeSet::OnRep_Atk_PostAdd(const FGameplayAttributeData& OldAtk_PostAdd)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Atk_PostAdd, OldAtk_PostAdd);
}

void URPGAttributeSet::OnRep_Def_PostAdd(const FGameplayAttributeData& OldDef_PostAdd)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Def_PostAdd, OldDef_PostAdd);
}

void URPGAttributeSet::OnRep_Hp_Mul(const FGameplayAttributeData& OldHp_Mul)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Hp_Mul, OldHp_Mul);
}

void URPGAttributeSet::OnRep_Atk_Mul(const FGameplayAttributeData& OldAtk_Mul)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Atk_Mul, OldAtk_Mul);
}

void URPGAttributeSet::OnRep_Def_Mul(const FGameplayAttributeData& OldDef_Mul)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Def_Mul, OldDef_Mul);
}

void URPGAttributeSet::OnRep_Crit(const FGameplayAttributeData& OldCrit)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Crit, OldCrit);
}

void URPGAttributeSet::OnRep_CritDmg(const FGameplayAttributeData& OldCritDmg)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, CritDmg, OldCritDmg);
}

void URPGAttributeSet::OnRep_IgnDef(const FGameplayAttributeData& OldIgnDef)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, IgnDef, OldIgnDef);
}

void URPGAttributeSet::OnRep_PowerRegenRate(const FGameplayAttributeData& OldPowerRegenRate)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, PowerRegenRate, OldPowerRegenRate);
}

void URPGAttributeSet::OnRep_BreakBonus(const FGameplayAttributeData& OldBreakBonus)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, BreakBonus, OldBreakBonus);
}

void URPGAttributeSet::OnRep_Dmg_Mul(const FGameplayAttributeData& OldDmg_Mul)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Dmg_Mul, OldDmg_Mul);
}

void URPGAttributeSet::OnRep_DmgRed_Mul(const FGameplayAttributeData& OldDmgRed_Mul)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, DmgRed_Mul, OldDmgRed_Mul);
}

void URPGAttributeSet::OnRep_HealthSteal(const FGameplayAttributeData& OldHealthSteal)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, HealthSteal, OldHealthSteal);
}

void URPGAttributeSet::OnRep_NormalSkillSpeed(const FGameplayAttributeData& OldNormalSkillSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, NormalSkillSpeed, OldNormalSkillSpeed);
}

void URPGAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, MoveSpeed, OldMoveSpeed);

    // 当移动速度变化时，同步到CharacterMovementComponent
    if (AActor* Owner = GetOwningActor())
    {
        if (ACharacter* Character = Cast<ACharacter>(Owner))
        {
            if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
            {
                MovementComp->MaxWalkSpeed = GetMoveSpeed();
            }
        }
    }
}
