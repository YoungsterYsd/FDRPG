#include "RPGAbilityTagRelationshipMapping.h"

void URPGAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const
{
	// 遍历所有关系条目，如果能力标签集合中包含该条目的 AbilityTag，则收集其对应的 Block/Cancel 标签
	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FRPGAbilityTagRelationship& Tags = AbilityTagRelationships[i];
		if (AbilityTags.HasTag(Tags.AbilityTag))
		{
			if (OutTagsToBlock)
			{
				OutTagsToBlock->AppendTags(Tags.AbilityTagsToBlock);
			}
			if (OutTagsToCancel)
			{
				OutTagsToCancel->AppendTags(Tags.AbilityTagsToCancel);
			}
		}
	}
}

void URPGAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const
{
	// 遍历所有关系条目，如果能力标签集合中包含该条目的 AbilityTag，则收集其隐式的激活约束
	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FRPGAbilityTagRelationship& Tags = AbilityTagRelationships[i];
		if (AbilityTags.HasTag(Tags.AbilityTag))
		{
			if (OutActivationRequired)
			{
				OutActivationRequired->AppendTags(Tags.ActivationRequiredTags);
			}
			if (OutActivationBlocked)
			{
				OutActivationBlocked->AppendTags(Tags.ActivationBlockedTags);
			}
		}
	}
}

bool URPGAbilityTagRelationshipMapping::IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const
{
	// 检查是否存在某个关系条目：其 AbilityTag == ActionTag，且其 Cancel 列表包含能力标签集合中的任一标签
	for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
	{
		const FRPGAbilityTagRelationship& Tags = AbilityTagRelationships[i];

		if (Tags.AbilityTag == ActionTag && Tags.AbilityTagsToCancel.HasAny(AbilityTags))
		{
			return true;
		}
	}

	return false;
}
