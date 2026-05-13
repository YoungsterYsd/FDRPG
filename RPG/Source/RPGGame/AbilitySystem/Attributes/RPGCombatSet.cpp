// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGCombatSet.h"

#include "AbilitySystem/Attributes/RPGAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGCombatSet)

class FLifetimeProperty;


URPGCombatSet::URPGCombatSet()
	: BaseDamage(0.0f)
	, BaseHeal(0.0f)
{
}

void URPGCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URPGCombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGCombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
}

void URPGCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGCombatSet, BaseDamage, OldValue);
}

void URPGCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGCombatSet, BaseHeal, OldValue);
}

