// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "NativeGameplayTags.h"

namespace RPGGameplayTags
{
	RPGGAME_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	// Declare all of the custom native tags that Lyra will use
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);

	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);

	// === A2 锁定：14 个 RPG InputAction 对应的 InputTag ===
	// 移动 / 全局原生输入（NativeInputActions，C++ Bind）
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);                  // ↑↓←→ / WASD 备选
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Native_Pickup);         // F  拾取/交互
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Native_SwitchWeapon);   // Tab 切换主副手
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Native_OpenMenu);       // Esc 打开主菜单
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Native_OpenInventory);  // I   打开背包

	// 战斗能力输入（AbilityInputActions，自动绑定到 GameplayAbility）
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Attack);        // 鼠标左键 / A 主手普攻
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Q);             // Q 主手技能 1
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_W);             // W 主手技能 2
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_E);             // E 副手技能 3
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_R);             // R 副手技能 4
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Ultimate);      // Space 当前主手大招
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Dodge);         // Shift 闪避
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_Block);         // Ctrl 格挡
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_HealItem);      // H 使用恢复品

	// === Lyra 残留：保留以兼容 HeroComponent 的 Bind 逻辑（A4 切俯视角后再决定是否清理）===
	// [A4 待清理] 以下 4 个 InputTag 在 RPGHeroComponent.cpp 中 BindNativeAction，A4 俯视角改造时连同 LookMouse/LookStick/Crouch/AutoRun 输入处理一起删除
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Stick);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_AutoRun);

	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);

	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Reset);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_RequestReset);

	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Heal);

	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_GodMode);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_UnlimitedHealth);

	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Crouching);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_AutoRunning);
	// [A4 待清理] Status_Crouching / Status_AutoRunning 为 Lyra 蹲伏/自动跑残留状态 Tag，A4 俯视角改造时一并删除（涉及 RPGCharacter::OnStartCrouch / RPGPlayerController::OnStartAutoRun）
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);

	// These are mappings from MovementMode enums to GameplayTags associated with those enums (below)
	RPGGAME_API	extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
	RPGGAME_API	extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;

	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_NavWalking);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);

	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Custom);

	// =====================================================================================
	// === A3 v7 锁定：RPG GAS 属性 / SetByCaller / 战斗状态 / 消息 Tag ===
	// 命名规范：[目标在前，修饰在后]，详见 14_后续日程与验收清单.md §3.1.2
	// =====================================================================================

	// --- A3 Attribute Tag（HealthSet 6 字段，AttributeBased / UMG 绑定用） ---
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Health_Final);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Health_Max);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Health_Healing);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Health_Damage);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Stamina_Current);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Stamina_Max);

	// --- A3 Attribute Tag（PrimaryAttributeSet 21 字段） ---
	// 组 1：基础三维（*Base）
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Health_Base);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Attack_Base);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Defense_Base);
	// 组 2：加性加成（*Bonus）
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Health_Bonus);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Attack_Bonus);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Defense_Bonus);
	// 组 3：百分比乘数（*Mul）
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Health_Mul);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Attack_Mul);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Defense_Mul);
	// 组 4：汇总派生（*Final）—— MaxHealth 的派生写到 HealthSet
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Attack_Final);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Defense_Final);
	// 组 5：战斗倍率
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Crit_Chance);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Crit_Damage);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Defense_Penetration);
	// 组 6：伤害修正
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Damage_Bonus);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Damage_Reduction);
	// 组 7：特殊
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_LifeSteal);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_EnergyGainMul);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_BreakBonus);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_AttackSpeed);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_MoveSpeed);

	// --- A3 SetByCaller Tag（跨 GE/GA 传参） ---
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage_Base);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage_AttackerScalingCoef);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Block_DamageReduction);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Block_StaminaConsume);

	// --- A3 State Tag（布尔状态） ---
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Combat_Active);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Block_Blocking);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Block_PerfectBlocking);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Block_Broken);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stamina_RegenOutOfCombat);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stamina_RegenInCombat);

	// --- A3 Message Tag（GameplayMessage 广播） ---
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Damage_Popup);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Block_PerfectTriggered);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Block_Broken);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Health_OutOfHealth);
};
