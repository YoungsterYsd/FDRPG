// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGInputConfig.h"
#include "Engine/AssetManager.h"
#include "GameplayTagContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGInputConfig)

URPGInputConfig::URPGInputConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const FGameplayTag& URPGInputConfig::FindInputTagForAction(const UInputAction* InputAction) const
{
	for (const FRPGInputAction& Input : AbilityInputActions)
	{
		if (Input.InputAction == InputAction)
		{
			return Input.InputTag;
		}
	}

	return FGameplayTag::EmptyTag;
}
