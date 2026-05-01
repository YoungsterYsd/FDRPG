// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "RPGAbilityTagRelationshipMapping.generated.h"

/**
 * FRPGAbilityTagRelationship
 *
 *	Defines the relationship between an ability tag and other tags
 *	(blocking, canceling, etc.)
 */
USTRUCT()
struct FRPGAbilityTagRelationship
{
	GENERATED_BODY()

public:

	// The tag this relationship is for
	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (Categories = "AbilityTag"))
	FGameplayTag AbilityTag;

	// Tags that this ability blocks from activating
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FGameplayTagContainer AbilityTagsToBlock;

	// Tags that this ability cancels when activated
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FGameplayTagContainer AbilityTagsToCancel;

	// Tags that are required to activate this ability
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FGameplayTagContainer RequiredTags;

	// Tags that block this ability from activating
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	FGameplayTagContainer BlockedTags;
};

/**
 * URPGAbilityTagRelationshipMapping
 *
 *	能力 Tag 关系映射数据资产，定义能力之间的阻断/取消关系。
 *	用于 ASC 判断哪些能力可以同时激活（P2 完整实现）。
 */
UCLASS(Const, Meta = (DisplayName = "RPG Ability Tag Relationship Mapping"))
class RPGGAME_API URPGAbilityTagRelationshipMapping : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	URPGAbilityTagRelationshipMapping();

	// Get tags that should be blocked by the given ability tags
	void GetAbilityTagsToBlock(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutTagsToBlock) const;

	// Get tags that should be canceled by the given ability tags
	void GetAbilityTagsToCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutTagsToCancel) const;

	// Get required tags for the given ability tags
	void GetRequiredTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutRequiredTags) const;

	// Get blocked tags for the given ability tags
	void GetBlockedTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutBlockedTags) const;

protected:

	// The list of ability tag relationships
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TArray<FRPGAbilityTagRelationship> TagRelationships;
};
