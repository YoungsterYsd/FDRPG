// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGAbilitySystemComponent.h"
#include "AbilitySystem/RPGGameplayAbility.h"
#include "GameplayTags/RPGGameplayTags.h"
#include "System/RPGLogChannels.h"
#include "Abilities/GameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGAbilitySystemComponent)

URPGAbilitySystemComponent::URPGAbilitySystemComponent()
{
	// Enable ticking so ProcessAbilityInput is called every frame
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void URPGAbilitySystemComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Process ability input on the authority or locally controlled client
	AActor* Avatar = GetAvatarActor();
	const APawn* AvatarPawn = Cast<APawn>(Avatar);
	const bool bIsLocal = AvatarPawn && AvatarPawn->IsLocallyControlled();
	if (GetOwnerActor()->HasAuthority() || bIsLocal)
	{
		const bool bGamePaused = Avatar && Avatar->GetWorld() && Avatar->GetWorld()->IsPaused();
		ProcessAbilityInput(DeltaTime, bGamePaused);
	}
}

void URPGAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	// Try to activate all abilities with OnSpawn policy
	TryActivateAbilitiesOnSpawn();
}

void URPGAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (const URPGGameplayAbility* RPGAbilityCDO = Cast<URPGGameplayAbility>(AbilitySpec.Ability))
		{
			if (RPGAbilityCDO->GetActivationPolicy() == ERPGAbilityActivationPolicy::OnSpawn)
			{
				TryActivateAbility(AbilitySpec.Handle, false);
			}
		}
	}
}

void URPGAbilitySystemComponent::CancelAbilitiesByFunc(
	TFunctionRef<bool(const URPGGameplayAbility* Ability, FGameplayAbilitySpecHandle Handle)> ShouldCancelFunc,
	const FGameplayTagContainer* WithTags, const FGameplayTagContainer* WithoutTags,
	UGameplayAbility* IgnoreAbility)
{
	ABILITYLIST_SCOPE_LOCK();

	TArray<FGameplayAbilitySpecHandle> AbilitiesToCancel;
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.IsActive())
		{
			continue;
		}

		if (IgnoreAbility && (IgnoreAbility == AbilitySpec.Ability || IgnoreAbility->GetCurrentAbilitySpec() == &AbilitySpec))
		{
			continue;
		}

		if (WithTags && !AbilitySpec.Ability->AbilityTags.HasAll(*WithTags))
		{
			continue;
		}

		if (WithoutTags && AbilitySpec.Ability->AbilityTags.HasAny(*WithoutTags))
		{
			continue;
		}

		if (const URPGGameplayAbility* RPGAbilityCDO = Cast<URPGGameplayAbility>(AbilitySpec.Ability))
		{
			if (ShouldCancelFunc(RPGAbilityCDO, AbilitySpec.Handle))
			{
				AbilitiesToCancel.Add(AbilitySpec.Handle);
			}
		}
	}

	for (FGameplayAbilitySpecHandle& HandleToCancel : AbilitiesToCancel)
	{
		CancelAbilityHandle(HandleToCancel);
	}
}

void URPGAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		UE_LOG(LogRPG, Log, TEXT("URPGAbilitySystemComponent::OnAbilityInputPressed - InputTag: %s, ActivatableAbilities: %d"),
			*InputTag.ToString(), ActivatableAbilities.Items.Num());

		int32 MatchCount = 0;
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
			{
				continue;
			}

			MatchCount++;
			InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
			InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
		}

		UE_LOG(LogRPG, Log, TEXT("  Matched %d abilities for InputTag %s"), MatchCount, *InputTag.ToString());
	}
}

void URPGAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
			{
				continue;
			}

			InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
			InputHeldSpecHandles.Remove(AbilitySpec.Handle);
		}
	}
}

void URPGAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;

	// Process all abilities that had their input pressed this frame
	AbilitiesToActivate.Reset();
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (const URPGGameplayAbility* RPGAbilityCDO = Cast<URPGGameplayAbility>(AbilitySpec->Ability))
			{
				if (RPGAbilityCDO->GetActivationPolicy() == ERPGAbilityActivationPolicy::OnInputTriggered)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	// Try to activate all pressed abilities
	for (FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle, false);
	}

	// Process all abilities that are held (WhileInputActive policy)
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (const URPGGameplayAbility* RPGAbilityCDO = Cast<URPGGameplayAbility>(AbilitySpec->Ability))
			{
				if (RPGAbilityCDO->GetActivationPolicy() == ERPGAbilityActivationPolicy::WhileInputActive && !AbilitySpec->IsActive())
				{
					TryActivateAbility(AbilitySpec->Handle, false);
				}
			}
		}
	}

	// Process all abilities that had their input released this frame
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->IsActive())
			{
				// Tell instanced abilities that the input was released
				TArray<UGameplayAbility*> Instances = AbilitySpec->GetAbilityInstances();
				for (UGameplayAbility* AbilityInstance : Instances)
				{
					if (AbilityInstance && AbilityInstance->GetCurrentActorInfo())
					{
						AbilityInstance->InputReleased(SpecHandle, AbilityInstance->GetCurrentActorInfo(), AbilityInstance->GetCurrentActivationInfo());
					}
				}
			}
		}
	}

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

FGameplayAbilitySpec* URPGAbilitySystemComponent::FindAbilitySpecForInputTag(const FGameplayTag& InputTag)
{
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

const FRPGGameplayTargetDataInfo* URPGAbilitySystemComponent::GetTargetDataInfoForAbility(FGameplayAbilitySpecHandle AbilityHandle) const
{
	return AbilityTargetDataMap.Find(AbilityHandle);
}

FGameplayTag URPGAbilitySystemComponent::GetFirstPressedInputTag() const
{
	for (const FGameplayAbilitySpecHandle& Handle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle))
		{
			for (const FGameplayTag& Tag : Spec->GetDynamicSpecSourceTags())
			{
				if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
				{
					return Tag;
				}
			}
		}
	}
	return FGameplayTag();
}
