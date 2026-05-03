// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"
#include "RPGLogChannels.h"
#include "NativeGameplayTags.h"

namespace RPGplayTags
{
	RPG_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
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

	//=====================================================================
	// Ability - 能力激活失败
	//=====================================================================
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	//=====================================================================
	// Ability - 行为标记
	//=====================================================================
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);

	//=====================================================================
	// Input - 输入映射 
	//=====================================================================
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Dash);
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Block);
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_WeaponSwitch);
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Interact);

	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_NormalAttack);
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Skill1);
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Skill2);
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Skill3);
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Skill4);
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ultimate);
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_MouseLeftClick);
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_MouseRightClick);
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_MouseMove);

	// 测试用InputTag
	RPG_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_TestA);
}
