// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RPGAbilityCost.h"
#include "ScalableFloat.h"
#include "Templates/SubclassOf.h"

#include "RPGAbilityCost_InventoryItem.generated.h"

struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpecHandle;

class URPGGameplayAbility;
class URPGInventoryItemDefinition;
class UObject;
struct FGameplayAbilityActorInfo;
struct FGameplayTagContainer;

/**
 * Represents a cost that requires expending a quantity of an inventory item
 */
UCLASS(meta=(DisplayName="Inventory Item"))
class URPGAbilityCost_InventoryItem : public URPGAbilityCost
{
	GENERATED_BODY()

public:
	URPGAbilityCost_InventoryItem();

	//~ULyraAbilityCost interface
	virtual bool CheckCost(const URPGGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const URPGGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	//~End of ULyraAbilityCost interface

protected:
	/** How much of the item to spend (keyed on ability level) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost)
	FScalableFloat Quantity;

	/** Which item to consume */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost)
	TSubclassOf<URPGInventoryItemDefinition> ItemDefinition;
};
