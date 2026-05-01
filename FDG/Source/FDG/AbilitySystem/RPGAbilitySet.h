// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "RPGAbilitySet.generated.h"

class UGameplayEffect;
class UAttributeSet;
class URPGAbilitySystemComponent;
class URPGGameplayAbility;

/**
 * FRPGAbilitySetAbilityEntry
 *
 *	Defines a single ability entry in an ability set.
 */
USTRUCT(BlueprintType)
struct FRPGAbilitySetAbilityEntry
{
	GENERATED_BODY()

public:

	// The ability to grant
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<UGameplayAbility> Ability = nullptr;

	// Input tag to associate with the ability
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	FGameplayTag InputTag;

	// Ability level to grant
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	int32 AbilityLevel = 1;
};

/**
 * FRPGAbilitySetEffectEntry
 *
 *	Defines a single gameplay effect entry in an ability set.
 */
USTRUCT(BlueprintType)
struct FRPGAbilitySetEffectEntry
{
	GENERATED_BODY()

public:

	// The gameplay effect to grant
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	// Effect level to grant
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	int32 EffectLevel = 1;
};

/**
 * FRPGAbilitySet_GrantedHandles
 *
 *	Tracks the handles for abilities, effects, and attribute sets that were granted.
 *	Supports removing them later.
 */
USTRUCT(BlueprintType)
struct FRPGAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);

	// Remove all granted handles from the given ASC
	void RemoveFromAbilitySystem(URPGAbilitySystemComponent* ASC) const;

	// Returns true if any handles are stored
	bool IsValid() const;

private:

	// Granted ability spec handles
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// Granted gameplay effect handles
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// Granted attribute sets
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

/**
 * URPGAbilitySet
 *
 *	能力集数据资产，定义一组可授予 ASC 的能力、效果和属性集。
 *	PawnData 通过 TSoftObjectPtr 引用 AbilitySet，在初始化时授予给角色。
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "RPG Ability Set", ShortTooltip = "Data asset defining a set of abilities, effects, and attribute sets."))
class RPGGAME_API URPGAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	URPGAbilitySet();

	// Grant all abilities, effects, and attribute sets from this set to the given ASC
	void GiveToAbilitySystem(URPGAbilitySystemComponent* ASC, FRPGAbilitySet_GrantedHandles* OutGrantedHandles = nullptr, UObject* SourceObject = nullptr) const;

	// The abilities to grant
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilitySet")
	TArray<FRPGAbilitySetAbilityEntry> Abilities;

	// The gameplay effects to grant
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilitySet")
	TArray<FRPGAbilitySetEffectEntry> Effects;

	// The attribute sets to grant
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilitySet")
	TArray<TSubclassOf<UAttributeSet>> AttributeSets;
};
