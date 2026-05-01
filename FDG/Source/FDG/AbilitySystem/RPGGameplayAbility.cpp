// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGGameplayAbility.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "GameplayTags/RPGGameplayTags.h"
#include "System/RPGLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGGameplayAbility)

URPGGameplayAbility::URPGGameplayAbility()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

	// Default to not replicating the avatar
	bReplicateInputDirectly = false;
}

void URPGGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	// 将 InputTag 写入 AbilitySpec 的 DynamicSpecSourceTags，使 ASC 能按 Tag 匹配能力
	if (InputTag.IsValid() && !Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
	{
		const_cast<FGameplayAbilitySpec&>(Spec).GetDynamicSpecSourceTags().AddTag(InputTag);
	}
}

void URPGGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
}

bool URPGGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}
