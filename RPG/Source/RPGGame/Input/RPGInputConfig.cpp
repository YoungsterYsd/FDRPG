// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGInputConfig.h"

#include "RPGLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGInputConfig)


URPGInputConfig::URPGInputConfig(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* URPGInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FRPGInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogLyra, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* URPGInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FRPGInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogLyra, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

void URPGInputConfig::AddNativeMapping(const UInputAction* InputAction, FName InputTagName)
{
	FRPGInputAction NewEntry;
	NewEntry.InputAction = InputAction;
	NewEntry.InputTag    = FGameplayTag::RequestGameplayTag(InputTagName, /*ErrorIfNotFound=*/false);
	NativeInputActions.Add(NewEntry);
}

void URPGInputConfig::AddAbilityMapping(const UInputAction* InputAction, FName InputTagName)
{
	FRPGInputAction NewEntry;
	NewEntry.InputAction = InputAction;
	NewEntry.InputTag    = FGameplayTag::RequestGameplayTag(InputTagName, /*ErrorIfNotFound=*/false);
	AbilityInputActions.Add(NewEntry);
}

void URPGInputConfig::ClearAllMappings()
{
	NativeInputActions.Empty();
	AbilityInputActions.Empty();
}
