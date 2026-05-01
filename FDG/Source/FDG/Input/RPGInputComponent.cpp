// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGInputComponent.h"
#include "Input/RPGInputConfig.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "System/RPGLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGInputComponent)

URPGInputComponent::URPGInputComponent()
{
}

void URPGInputComponent::BindAbilityActions(const URPGInputConfig* InputConfig, URPGAbilitySystemComponent* ASC, TArray<uint32>& BindHandles)
{
	check(InputConfig);
	check(ASC);

	int32 BoundCount = 0;
	int32 SkippedCount = 0;
	for (const FRPGInputAction& Action : InputConfig->InputActions)
	{
		if (!Action.bIsNative && Action.InputTag.IsValid())
		{
			const UInputAction* IA = Action.InputAction.Get();
			if (!IA && !Action.InputAction.IsNull())
			{
				// Soft pointer not loaded yet, force load
				IA = Action.InputAction.LoadSynchronous();
			}

			if (IA)
			{
				// Bind pressed event - capture the InputTag by value and forward to ASC
				FGameplayTag InputTag = Action.InputTag;

				FEnhancedInputActionEventBinding& PressedBinding = BindActionInstanceLambda(IA, ETriggerEvent::Triggered,
					[ASC, InputTag](const FInputActionInstance& ActionInstance)
					{
						ASC->OnAbilityInputPressed(InputTag);
					});
				BindHandles.Add(PressedBinding.GetHandle());

				// Bind released event
				FEnhancedInputActionEventBinding& ReleasedBinding = BindActionInstanceLambda(IA, ETriggerEvent::Completed,
					[ASC, InputTag](const FInputActionInstance& ActionInstance)
					{
						ASC->OnAbilityInputReleased(InputTag);
					});
				BindHandles.Add(ReleasedBinding.GetHandle());

				BoundCount++;

				UE_LOG(LogRPG, Log, TEXT("  Bound ability input: IA='%s' -> Tag='%s'"), *GetNameSafe(IA), *InputTag.ToString());
			}
			else
			{
				SkippedCount++;
				UE_LOG(LogRPG, Warning, TEXT("URPGInputComponent::BindAbilityActions - InputAction not loaded for tag %s (path: %s)"),
					*Action.InputTag.ToString(), *Action.InputAction.ToString());
			}
		}
	}

	UE_LOG(LogRPG, Log, TEXT("URPGInputComponent::BindAbilityActions - Bound %d ability actions, skipped %d from config '%s'"),
		BoundCount, SkippedCount, *GetNameSafe(InputConfig));
}

void URPGInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
