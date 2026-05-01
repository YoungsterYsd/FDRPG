// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGAbilityTagRelationshipMapping.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGAbilityTagRelationshipMapping)

URPGAbilityTagRelationshipMapping::URPGAbilityTagRelationshipMapping()
{
}

void URPGAbilityTagRelationshipMapping::GetAbilityTagsToBlock(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutTagsToBlock) const
{
	for (const FRPGAbilityTagRelationship& Relationship : TagRelationships)
	{
		if (AbilityTags.HasTag(Relationship.AbilityTag))
		{
			OutTagsToBlock.AppendTags(Relationship.AbilityTagsToBlock);
		}
	}
}

void URPGAbilityTagRelationshipMapping::GetAbilityTagsToCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutTagsToCancel) const
{
	for (const FRPGAbilityTagRelationship& Relationship : TagRelationships)
	{
		if (AbilityTags.HasTag(Relationship.AbilityTag))
		{
			OutTagsToCancel.AppendTags(Relationship.AbilityTagsToCancel);
		}
	}
}

void URPGAbilityTagRelationshipMapping::GetRequiredTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutRequiredTags) const
{
	for (const FRPGAbilityTagRelationship& Relationship : TagRelationships)
	{
		if (AbilityTags.HasTag(Relationship.AbilityTag))
		{
			OutRequiredTags.AppendTags(Relationship.RequiredTags);
		}
	}
}

void URPGAbilityTagRelationshipMapping::GetBlockedTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutBlockedTags) const
{
	for (const FRPGAbilityTagRelationship& Relationship : TagRelationships)
	{
		if (AbilityTags.HasTag(Relationship.AbilityTag))
		{
			OutBlockedTags.AppendTags(Relationship.BlockedTags);
		}
	}
}
