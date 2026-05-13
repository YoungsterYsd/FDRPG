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

	// === Lyra 残留：暂保留以兼容 HeroComponent 的 Bind 逻辑（A4 切俯视角后再决定是否清理）===
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
};
