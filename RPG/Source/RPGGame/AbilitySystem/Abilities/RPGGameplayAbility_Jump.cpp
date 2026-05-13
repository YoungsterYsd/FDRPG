// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGGameplayAbility_Jump.h"

#include "AbilitySystem/Abilities/RPGGameplayAbility.h"
#include "Character/RPGCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGGameplayAbility_Jump)

struct FGameplayTagContainer;


URPGGameplayAbility_Jump::URPGGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool URPGGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const ARPGCharacter* RPGCharacter = Cast<ARPGCharacter>(ActorInfo->AvatarActor.Get());
	if (!RPGCharacter || !RPGCharacter->CanJump())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void URPGGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Stop jumping in case the ability blueprint doesn't call it.
	CharacterJumpStop();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void URPGGameplayAbility_Jump::CharacterJumpStart()
{
	if (ARPGCharacter* RPGCharacter = GetRPGCharacterFromActorInfo())
	{
		if (RPGCharacter->IsLocallyControlled() && !RPGCharacter->bPressedJump)
		{
			RPGCharacter->UnCrouch();
			RPGCharacter->Jump();
		}
	}
}

void URPGGameplayAbility_Jump::CharacterJumpStop()
{
	if (ARPGCharacter* RPGCharacter = GetRPGCharacterFromActorInfo())
	{
		if (RPGCharacter->IsLocallyControlled() && RPGCharacter->bPressedJump)
		{
			RPGCharacter->StopJumping();
		}
	}
}

