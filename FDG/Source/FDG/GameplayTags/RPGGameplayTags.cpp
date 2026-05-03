// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGGameplayTags.h"

#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"
#include "System/RPGLogChannels.h"

namespace RPGGameplayTags
{
	//=====================================================================
	// Ability - 能力激活失败
	//=====================================================================
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_IsDead, "Ability.ActivateFail.IsDead", "Ability failed to activate because its owner is dead.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Cooldown, "Ability.ActivateFail.Cooldown", "Ability failed to activate because it is on cool down.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Cost, "Ability.ActivateFail.Cost", "Ability failed to activate because it did not pass the cost checks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_TagsBlocked, "Ability.ActivateFail.TagsBlocked", "Ability failed to activate because tags are blocking it.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_TagsMissing, "Ability.ActivateFail.TagsMissing", "Ability failed to activate because tags are missing.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Networking, "Ability.ActivateFail.Networking", "Ability failed to activate because it did not pass the network checks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_ActivationGroup, "Ability.ActivateFail.ActivationGroup", "Ability failed to activate because of its activation group.");

	//=====================================================================
	// Ability - 行为标记
	//=====================================================================
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Behavior_SurvivesDeath, "Ability.Behavior.SurvivesDeath", "An ability with this type tag should not be canceled due to death.");

	//=====================================================================
	// Input - 输入映射 (Lyra 基础)
	//=====================================================================
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Move, "InputTag.Move", "Move input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Look_Mouse, "InputTag.Look.Mouse", "Look (mouse) input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Look_Stick, "InputTag.Look.Stick", "Look (stick) input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Crouch, "InputTag.Crouch", "Crouch input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_AutoRun, "InputTag.AutoRun", "Auto-run input.");

	//=====================================================================
	// Input - 输入映射 (RPG 扩展)
	//=====================================================================
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Dash, "InputTag.Dash", "Dash/Dodge input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Block, "InputTag.Block", "Block input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_WeaponSwitch, "InputTag.WeaponSwitch", "Weapon switch input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Interact, "InputTag.Interact", "Interact input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_NormalAttack, "InputTag.NormalAttack", "Normal attack input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Skill1, "InputTag.Skill1", "Skill 1 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Skill2, "InputTag.Skill2", "Skill 2 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Skill3, "InputTag.Skill3", "Skill 3 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Skill4, "InputTag.Skill4", "Skill 4 input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ultimate, "InputTag.Ultimate", "Ultimate input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_MouseLeftClick, "InputTag.MouseLeftClick", "Mouse left click for UI interaction and backup action.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_MouseRightClick, "InputTag.MouseRightClick", "Mouse right click for UI interaction and backup action.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_MouseMove, "InputTag.MouseMove", "Mouse movement for cursor tracking and backup look.");

	//=====================================================================
	// InitState - 初始化状态机
	//=====================================================================
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_Spawned, "InitState.Spawned", "1: Actor/component has initially spawned and can be extended");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataAvailable, "InitState.DataAvailable", "2: All required data has been loaded/replicated and is ready for initialization");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataInitialized, "InitState.DataInitialized", "3: The available data has been initialized for this actor/component, but it is not ready for full gameplay");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_GameplayReady, "InitState.GameplayReady", "4: The actor/component is fully ready for active gameplay");

	//=====================================================================
	// GameplayEvent - 游戏事件
	//=====================================================================
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Death, "GameplayEvent.Death", "Event that fires on death. This event only fires on the server.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Reset, "GameplayEvent.Reset", "Event that fires once a player reset is executed.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_RequestReset, "GameplayEvent.RequestReset", "Event to request a player's pawn to be instantly replaced with a new one at a valid spawn location.");

	//=====================================================================
	// SetByCaller - GE 数值传递
	//=====================================================================
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage, "SetByCaller.Damage", "SetByCaller tag used by damage gameplay effects.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Heal, "SetByCaller.Heal", "SetByCaller tag used by healing gameplay effects.");

	//=====================================================================
	// Cheat - 调试
	//=====================================================================
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cheat_GodMode, "Cheat.GodMode", "GodMode cheat is active on the owner.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cheat_UnlimitedHealth, "Cheat.UnlimitedHealth", "UnlimitedHealth cheat is active on the owner.");

	//=====================================================================
	// Status - 角色状态
	//=====================================================================
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Crouching, "Status.Crouching", "Target is crouching.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_AutoRunning, "Status.AutoRunning", "Target is auto-running.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death, "Status.Death", "Target has the death status.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death_Dying, "Status.Death.Dying", "Target has begun the death process.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death_Dead, "Status.Death.Dead", "Target has finished the death process.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Dashing, "Status.Dashing", "Target is dashing/dodging.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Blocking, "Status.Blocking", "Target is blocking.");

	//=====================================================================
	// Movement - 移动模式
	//=====================================================================
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Walking, "Movement.Mode.Walking", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_NavWalking, "Movement.Mode.NavWalking", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Falling, "Movement.Mode.Falling", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Swimming, "Movement.Mode.Swimming", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Flying, "Movement.Mode.Flying", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Custom, "Movement.Mode.Custom", "This is invalid and should be replaced with custom tags.  See RPGGameplayTags::CustomMovementModeTagMap.");

	//=====================================================================
	// RPG - 免疫/无敌帧
	//=====================================================================
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(RPG_Immunity_Dash, "RPG.Immunity.Dash", "Target is immune during dash/dodge (i-frames).");

	//=====================================================================
	// Attribute - 属性标记
	//=====================================================================
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Health, "RPG.Attribute.Health", "Health attribute tag.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Energy, "RPG.Attribute.Energy", "Energy attribute tag.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Attack, "RPG.Attribute.Attack", "Attack attribute tag.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Defense, "RPG.Attribute.Defense", "Defense attribute tag.");

	//=====================================================================
	// Status - 战斗状态（P2 补充）
	//=====================================================================
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Invulnerable, "Status.Invulnerable", "Target is invulnerable to damage.");

	//=====================================================================
	// Damage - 伤害类型
	//=====================================================================
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Physical, "RPG.Damage.Physical", "Physical damage type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Magical, "RPG.Damage.Magical", "Magical damage type.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_True, "RPG.Damage.True", "True damage (ignores defense).");

	//=====================================================================
	// Cooldown - 冷却标记
	//=====================================================================
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown_Attack, "RPG.Cooldown.Attack", "Attack cooldown.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown_Skill1, "RPG.Cooldown.Skill1", "Skill 1 cooldown.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown_Skill2, "RPG.Cooldown.Skill2", "Skill 2 cooldown.");

	// Unreal Movement Modes
	const TMap<uint8, FGameplayTag> MovementModeTagMap =
	{
		{ MOVE_Walking, Movement_Mode_Walking },
		{ MOVE_NavWalking, Movement_Mode_NavWalking },
		{ MOVE_Falling, Movement_Mode_Falling },
		{ MOVE_Swimming, Movement_Mode_Swimming },
		{ MOVE_Flying, Movement_Mode_Flying },
		{ MOVE_Custom, Movement_Mode_Custom }
	};

	// Custom Movement Modes
	const TMap<uint8, FGameplayTag> CustomMovementModeTagMap =
	{
		// Fill these in with your custom modes
	};

	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
	{
		const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

		if (!Tag.IsValid() && bMatchPartialString)
		{
			FGameplayTagContainer AllTags;
			Manager.RequestAllGameplayTags(AllTags, true);

			for (const FGameplayTag& TestTag : AllTags)
			{
				if (TestTag.ToString().Contains(TagString))
				{
					UE_LOG(LogRPG, Display, TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."), *TagString, *TestTag.ToString());
					Tag = TestTag;
					break;
				}
			}
		}

		return Tag;
	}
}
