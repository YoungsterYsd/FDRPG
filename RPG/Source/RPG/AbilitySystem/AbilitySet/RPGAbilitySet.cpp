// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGAbilitySet.h"
#include "../Core/RPGGameplayAbility.h"
#include "../Core/RPGAbilitySystemComponent.h"
#include "../AttributeSystem/RPGAttributeSet.h"
#include "../AttributeSystem/RPGAttributeInitTableRow.h"
#include "../../System/RPGLogChannels.h"

#include "Engine/DataTable.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGAbilitySet)

//////////////////////////////////////////////////////////////////////////
// FRPGAbilitySet_GrantedHandles
//////////////////////////////////////////////////////////////////////////

void FRPGAbilitySet_GrantedHandles::TakeFromAbilitySystem(URPGAbilitySystemComponent* ASC)
{
	check(ASC);

	// 移除授予的 AttributeSets
	for (TWeakObjectPtr<UAttributeSet>& AttributeSetPtr : AttributeSets)
	{
		if (AttributeSetPtr.IsValid())
		{
			ASC->RemoveSpawnedAttribute(AttributeSetPtr.Get());
		}
	}
	AttributeSets.Reset();

	// 结束并移除授予的 ActiveGameplayEffects
	for (FActiveGameplayEffectHandle& EffectHandle : EffectHandles)
	{
		if (EffectHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(EffectHandle);
		}
	}
	EffectHandles.Reset();

	// 移除授予的 Abilities
	for (FGameplayAbilitySpecHandle& AbilityHandle : AbilityHandles)
	{
		if (AbilityHandle.IsValid())
		{
			ASC->ClearAbility(AbilityHandle);
		}
	}
	AbilityHandles.Reset();
}

//////////////////////////////////////////////////////////////////////////
// URPGAbilitySet
//////////////////////////////////////////////////////////////////////////

bool URPGAbilitySet::GiveToAbilitySystem(URPGAbilitySystemComponent* ASC, FRPGAbilitySet_GrantedHandles* OutGrantedHandles) const
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		UE_LOG(LogRPGAbilitySystem, Warning, TEXT("GiveToAbilitySystem: ASC is not authoritative (only server can grant abilities)."));
		return false;
	}

	bool bAllSuccess = true;

	// ── 1. 授予 AttributeSets ──
	for (const FRPGAbilitySet_AttributeSet& AttributeSetToGrant : AttributeSets)
	{
		if (!IsValid(AttributeSetToGrant.AttributeSet))
		{
			UE_LOG(LogRPGAbilitySystem, Warning, TEXT("GiveToAbilitySystem: Invalid AttributeSet in AbilitySet [%s]"), *GetName());
			bAllSuccess = false;
			continue;
		}

		// 创建 AttributeSet 实例并添加到 ASC
		UAttributeSet* NewSet = NewObject<UAttributeSet>(ASC->GetOwnerActor(), AttributeSetToGrant.AttributeSet);
		ASC->AddAttributeSetSubobject(NewSet);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AttributeSets.Add(NewSet);
		}

		UE_LOG(LogRPGAbilitySystem, Display, TEXT("GiveToAbilitySystem: Granted AttributeSet [%s]"), *GetNameSafe(NewSet));
	}

	// ── 2. 授予 GameplayEffects ──
	for (const FRPGAbilitySet_GameplayEffect& EffectToGrant : GameplayEffects)
	{
		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogRPGAbilitySystem, Warning, TEXT("GiveToAbilitySystem: Invalid GameplayEffect in AbilitySet [%s]"), *GetName());
			bAllSuccess = false;
			continue;
		}

		// 创建 GE Spec 并应用到自身
		const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectToGrant.GameplayEffect, 1.0f, ASC->MakeEffectContext());
		const FActiveGameplayEffectHandle EffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		if (EffectHandle.IsValid())
		{
			if (OutGrantedHandles)
			{
				OutGrantedHandles->EffectHandles.Add(EffectHandle);
			}

			UE_LOG(LogRPGAbilitySystem, Display, TEXT("GiveToAbilitySystem: Applied GameplayEffect [%s]"), *GetNameSafe(EffectToGrant.GameplayEffect));
		}
		else
		{
			UE_LOG(LogRPGAbilitySystem, Error, TEXT("GiveToAbilitySystem: Failed to apply GameplayEffect [%s]"), *GetNameSafe(EffectToGrant.GameplayEffect));
			bAllSuccess = false;
		}
	}

	// ── 3. 授予 Abilities ──
	for (const FRPGAbilitySet_Ability& AbilityToGrant : Abilities)
	{
		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogRPGAbilitySystem, Warning, TEXT("GiveToAbilitySystem: Invalid Ability in AbilitySet [%s]"), *GetName());
			bAllSuccess = false;
			continue;
		}

		// 创建 AbilitySpec 并授予
		FGameplayAbilitySpec AbilitySpec(AbilityToGrant.Ability);
		const FGameplayAbilitySpecHandle AbilityHandle = ASC->GiveAbility(AbilitySpec);

		if (AbilityHandle.IsValid())
		{
			if (OutGrantedHandles)
			{
				OutGrantedHandles->AbilityHandles.Add(AbilityHandle);
			}

			UE_LOG(LogRPGAbilitySystem, Display, TEXT("GiveToAbilitySystem: Granted Ability [%s]"), *GetNameSafe(AbilityToGrant.Ability));

			// 如果配置了 InputTag，添加到 AbilitySpec 的 DynamicTags
			if (AbilityToGrant.InputTag.IsValid())
			{
				ASC->AddInputTagToAbilitySpec(AbilityHandle, AbilityToGrant.InputTag);
				
				UE_LOG(LogRPGAbilitySystem, Display, TEXT("GiveToAbilitySystem: Bound InputTag [%s] to Ability [%s]"), 
					*AbilityToGrant.InputTag.ToString(), *GetNameSafe(AbilityToGrant.Ability));
			}
		}
		else
		{
			UE_LOG(LogRPGAbilitySystem, Error, TEXT("GiveToAbilitySystem: Failed to grant Ability [%s]"), *GetNameSafe(AbilityToGrant.Ability));
			bAllSuccess = false;
		}
	}

	return bAllSuccess;
}
