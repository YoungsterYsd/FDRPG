// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGAttributeSet.h"

#include "AbilitySystem/RPGAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGAttributeSet)

class UWorld;


URPGAttributeSet::URPGAttributeSet()
{
}

UWorld* URPGAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

URPGAbilitySystemComponent* URPGAttributeSet::GetRPGAbilitySystemComponent() const
{
	return Cast<URPGAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

