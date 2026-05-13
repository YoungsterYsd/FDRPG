// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGAbilitySystemGlobals.h"

#include "RPGGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGAbilitySystemGlobals)

struct FGameplayEffectContext;

URPGAbilitySystemGlobals::URPGAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayEffectContext* URPGAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FRPGGameplayEffectContext();
}

