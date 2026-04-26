// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "NativeGameplayTags.h"

namespace RPGGameplayTags
{
	RPGGAME_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	//=====================================================================
	// Ability - 能力激活失败
	//=====================================================================
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);

	//=====================================================================
	// Ability - 行为标记
	//=====================================================================
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);

	//=====================================================================
	// Input - 输入映射 (Lyra 基础)
	//=====================================================================
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Stick);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_AutoRun);

	//=====================================================================
	// Input - 输入映射 (RPG 扩展)
	//=====================================================================
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Dash);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Block);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_WeaponSwitch);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Interact);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Skill1);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Skill2);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ultimate);

	//=====================================================================
	// InitState - 初始化状态机
	//=====================================================================
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);

	//=====================================================================
	// GameplayEvent - 游戏事件
	//=====================================================================
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Reset);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_RequestReset);

	//=====================================================================
	// SetByCaller - GE 数值传递
	//=====================================================================
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Heal);

	//=====================================================================
	// Cheat - 调试
	//=====================================================================
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_GodMode);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_UnlimitedHealth);

	//=====================================================================
	// Status - 角色状态
	//=====================================================================
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Crouching);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_AutoRunning);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Dashing);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Blocking);

	//=====================================================================
	// Movement - 移动模式
	//=====================================================================
	RPGGAME_API	extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
	RPGGAME_API	extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;

	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_NavWalking);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Custom);

	//=====================================================================
	// RPG - 免疫/无敌帧
	//=====================================================================
	RPGGAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(RPG_Immunity_Dash);
};
