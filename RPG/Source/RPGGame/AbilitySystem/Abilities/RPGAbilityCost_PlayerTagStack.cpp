// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGAbilityCost_PlayerTagStack.h"

#include "GameFramework/Controller.h"
#include "RPGGameplayAbility.h"
#include "Player/RPGPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGAbilityCost_PlayerTagStack)

URPGAbilityCost_PlayerTagStack::URPGAbilityCost_PlayerTagStack()
{
	Quantity.SetValue(1.0f);
}

bool URPGAbilityCost_PlayerTagStack::CheckCost(const URPGGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (AController* PC = Ability->GetControllerFromActorInfo())
	{
		if (ARPGPlayerState* PS = Cast<ARPGPlayerState>(PC->PlayerState))
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

			return PS->GetStatTagStackCount(Tag) >= NumStacks;
		}
	}
	return false;
}

void URPGAbilityCost_PlayerTagStack::ApplyCost(const URPGGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo->IsNetAuthority())
	{
		if (AController* PC = Ability->GetControllerFromActorInfo())
		{
			if (ARPGPlayerState* PS = Cast<ARPGPlayerState>(PC->PlayerState))
			{
				const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

				const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
				const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

				PS->RemoveStatTagStack(Tag, NumStacks);
			}
		}
	}
}

