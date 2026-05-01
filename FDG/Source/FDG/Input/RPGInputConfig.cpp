// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGInputConfig.h"
#include "System/RPGLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGInputConfig)

URPGInputConfig::URPGInputConfig()
{
}

const UInputAction* URPGInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FRPGInputAction& Action : InputActions)
	{
		if (Action.bIsNative && Action.InputTag == InputTag)
		{
			if (const UInputAction* IA = Action.InputAction.Get())
			{
				return IA;
			}
			// 软引用未加载时强制同步加载
			if (!Action.InputAction.IsNull())
			{
				return Action.InputAction.LoadSynchronous();
			}
		}
	}
	return nullptr;
}

const UInputAction* URPGInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FRPGInputAction& Action : InputActions)
	{
		if (!Action.bIsNative && Action.InputTag == InputTag)
		{
			if (const UInputAction* IA = Action.InputAction.Get())
			{
				return IA;
			}
			if (!Action.InputAction.IsNull())
			{
				return Action.InputAction.LoadSynchronous();
			}
		}
	}
	return nullptr;
}
