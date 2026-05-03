// Fill out your copyright notice in the Description page of Project Settings.

#include "RPGGameplayTags.h"

namespace RPGplayTags
{
#pragma region Ability - 能力激活失败
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_IsDead, "Ability.ActivateFail.IsDead");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_Cooldown, "Ability.ActivateFail.Cooldown");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_Cost, "Ability.ActivateFail.Cost");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_TagsBlocked, "Ability.ActivateFail.TagsBlocked");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ActivateFail_TagsMissing, "Ability.ActivateFail.TagsMissing");
#pragma endregion

#pragma region Ability - 行为标记
	UE_DEFINE_GAMEPLAY_TAG(Ability_Behavior_SurvivesDeath, "Ability.Behavior.SurvivesDeath");
#pragma endregion

#pragma region Input - 输入映射
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Dash, "InputTag.Dash");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Block, "InputTag.Block");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_WeaponSwitch, "InputTag.WeaponSwitch");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Interact, "InputTag.Interact");

	UE_DEFINE_GAMEPLAY_TAG(InputTag_NormalAttack, "InputTag.NormalAttack");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Skill1, "InputTag.Skill1");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Skill2, "InputTag.Skill2");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Skill3, "InputTag.Skill3");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Skill4, "InputTag.Skill4");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Ultimate, "InputTag.Ultimate");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_MouseLeftClick, "InputTag.MouseLeftClick");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_MouseRightClick, "InputTag.MouseRightClick");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_MouseMove, "InputTag.MouseMove");

	// 测试用InputTag
	UE_DEFINE_GAMEPLAY_TAG(InputTag_TestA, "InputTag.TestA");
#pragma endregion
} // namespace RPGplayTags
