#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "RPGAbilityTagRelationshipMapping.generated.h"

/**
 * FRPGAbilityTagRelationship
 *
 * 定义单个能力标签与其他标签的关系：
 * - 当一个能力拥有 AbilityTag 时，它会自动阻断/取消哪些其他能力
 * - 拥有此标签的能力还需要满足哪些额外激活条件
 *
 * 使用场景举例：
 *   AbilityTag = "Ability.Attack"
 *   AbilityTagsToBlock = "Ability.Move"        → 攻击时自动阻断移动
 *   AbilityTagsToCancel = "Ability.Interact"   → 攻击时自动取消交互
 *   ActivationRequiredTags = "State.HasWeapon" → 攻击需要持有武器
 *   ActivationBlockedTags = "State.Stunned"    → 眩晕时无法攻击
 */
USTRUCT()
struct FRPGAbilityTagRelationship
{
	GENERATED_BODY()

	/** 当前关系所针对的能力标签（单个标签，一个能力可以有多个这样的关系条目） */
	UPROPERTY(EditAnywhere, Category = Ability, meta = (Categories = "Gameplay.Action"))
	FGameplayTag AbilityTag;

	/** 拥有此标签的能力激活时，会阻断拥有这些标签的其他能力 */
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer AbilityTagsToBlock;

	/** 拥有此标签的能力激活时，会取消拥有这些标签的正在运行的能力 */
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer AbilityTagsToCancel;

	/** 拥有此标签的能力，隐式增加这些激活必需标签 */
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer ActivationRequiredTags;

	/** 拥有此标签的能力，隐式增加这些激活阻断标签 */
	UPROPERTY(EditAnywhere, Category = Ability)
	FGameplayTagContainer ActivationBlockedTags;
};


/**
 * URPGAbilityTagRelationshipMapping
 *
 * 标签关系映射表（数据资产），集中管理所有能力标签之间的互斥/依赖关系。
 *
 * 工作原理：
 *   1. ASC 在 ApplyAbilityBlockAndCancelTags 中查此表，扩展 Block/Cancel 标签
 *   2. ASC 在 GetAdditionalActivationTagRequirements 中查此表，添加额外激活约束
 *   3. 如果未设置（nullptr），则不添加额外约束，仅使用 GA 自带的标签配置
 *
 * 优势：将分散在每个 GA 中的标签关系集中到一张表中管理，便于策划调整和维护。
 */
UCLASS()
class URPGAbilityTagRelationshipMapping : public UDataAsset
{
	GENERATED_BODY()

private:
	/** 标签关系列表，每个条目定义一个 AbilityTag 与其他标签的关系 */
	UPROPERTY(EditAnywhere, Category = Ability, meta=(TitleProperty="AbilityTag"))
	TArray<FRPGAbilityTagRelationship> AbilityTagRelationships;

public:
	/**
	 * 根据能力标签集合，查找此映射表，收集需要阻断和取消的标签。
	 * 用于 ApplyAbilityBlockAndCancelTags 中扩展原始的 Block/Cancel 标签。
	 */
	void GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const;

	/**
	 * 根据能力标签集合，查找此映射表，收集额外的激活必需/阻断标签。
	 * 用于 GetAdditionalActivationTagRequirements 中添加隐式激活约束。
	 */
	void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const;

	/** 判断指定能力标签集合是否被 ActionTag 取消 */
	bool IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const;
};
