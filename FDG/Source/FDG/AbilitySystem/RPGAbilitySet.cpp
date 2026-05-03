// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGAbilitySet.h"
#include "RPGAbilitySystemComponent.h"
#include "RPGGameplayAbility.h"
#include "System/RPGLogChannels.h"
#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGAbilitySet)

//////////////////////////////////////////////////////////////////////////
// FRPGAbilitySet_GrantedHandles
//////////////////////////////////////////////////////////////////////////

void FRPGAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FRPGAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FRPGAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	if (Set)
	{
		GrantedAttributeSets.Add(Set);
	}
}

void FRPGAbilitySet_GrantedHandles::RemoveFromAbilitySystem(URPGAbilitySystemComponent* ASC) const
{
	check(ASC);

	// Remove abilities
	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}

	// Remove gameplay effects
	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	// Remove attribute sets (Note: this is typically not done, but we support it)
	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		if (Set)
		{
			// AttributeSets are owned by the ASC and cannot be easily removed at runtime.
			// In practice, you may want to just clear the granted handles rather than removing them.
			UE_LOG(LogRPG, Verbose, TEXT("FRPGAbilitySet_GrantedHandles::RemoveFromAbilitySystem - AttributeSet removal requested but not supported at runtime. Set: %s"), *GetNameSafe(Set));
		}
	}
}

bool FRPGAbilitySet_GrantedHandles::IsValid() const
{
	return (AbilitySpecHandles.Num() > 0 || GameplayEffectHandles.Num() > 0 || GrantedAttributeSets.Num() > 0);
}

//////////////////////////////////////////////////////////////////////////
// URPGAbilitySet
//////////////////////////////////////////////////////////////////////////

URPGAbilitySet::URPGAbilitySet()
{
}

void URPGAbilitySet::GiveToAbilitySystem(URPGAbilitySystemComponent* ASC, FRPGAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(ASC);

	// Grant abilities
	for (int32 AbilityIndex = 0; AbilityIndex < Abilities.Num(); ++AbilityIndex)
	{
		const FRPGAbilitySetAbilityEntry& Entry = Abilities[AbilityIndex];
		if (!Entry.Ability)
		{
			UE_LOG(LogRPG, Error, TEXT("URPGAbilitySet::GiveToAbilitySystem - Ability at index %d is null in set %s"), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		// Create the ability spec
		FGameplayAbilitySpec AbilitySpec(Entry.Ability, Entry.AbilityLevel, INDEX_NONE, SourceObject);

		// Add the input tag to the dynamic tags
		if (Entry.InputTag.IsValid())
		{
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(Entry.InputTag);
		}

		// Also set the input tag on the RPGGameplayAbility CDO if it has one
		if (URPGGameplayAbility* RPGAbilityCDO = Cast<URPGGameplayAbility>(AbilitySpec.Ability))
		{
			if (RPGAbilityCDO->GetAbilityInputTag().IsValid())
			{
				AbilitySpec.GetDynamicSpecSourceTags().AddTag(RPGAbilityCDO->GetAbilityInputTag());
			}
		}

		// Give the ability to the ASC
		FGameplayAbilitySpecHandle AbilityHandle = ASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilityHandle);
		}
	}

	// Grant gameplay effects
	for (int32 EffectIndex = 0; EffectIndex < Effects.Num(); ++EffectIndex)
	{
		const FRPGAbilitySetEffectEntry& Entry = Effects[EffectIndex];
		if (!Entry.GameplayEffect)
		{
			UE_LOG(LogRPG, Error, TEXT("URPGAbilitySet::GiveToAbilitySystem - GameplayEffect at index %d is null in set %s"), EffectIndex, *GetNameSafe(this));
			continue;
		}

		// Apply the gameplay effect
		FActiveGameplayEffectHandle EffectHandle = ASC->ApplyGameplayEffectToSelf(Entry.GameplayEffect.GetDefaultObject(), Entry.EffectLevel, ASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(EffectHandle);
		}
	}

	// Grant attribute sets (skip if already registered to avoid duplicates)
	for (int32 SetIndex = 0; SetIndex < AttributeSets.Num(); ++SetIndex)
	{
		const TSubclassOf<UAttributeSet>& SetClass = AttributeSets[SetIndex];
		if (!SetClass)
		{
			UE_LOG(LogRPG, Error, TEXT("URPGAbilitySet::GiveToAbilitySystem - AttributeSet at index %d is null in set %s"), SetIndex, *GetNameSafe(this));
			continue;
		}

		// 检查是否已存在同类型的 AttributeSet（避免 PlayerState 默认子对象重复注册）
		bool bAlreadyExists = false;
		for (UAttributeSet* ExistingSet : ASC->GetSpawnedAttributes())
		{
			if (ExistingSet && ExistingSet->IsA(SetClass))
			{
				bAlreadyExists = true;
				UE_LOG(LogRPG, Verbose, TEXT("URPGAbilitySet::GiveToAbilitySystem - AttributeSet '%s' already exists, skipping."), *GetNameSafe(ExistingSet));
				break;
			}
		}

		if (!bAlreadyExists)
		{
			UAttributeSet* NewSet = NewObject<UAttributeSet>(ASC->GetOwner(), SetClass);
			if (NewSet)
			{
				ASC->AddAttributeSetSubobject(NewSet);

				if (OutGrantedHandles)
				{
					OutGrantedHandles->AddAttributeSet(NewSet);
				}
			}
		}
	}
}
