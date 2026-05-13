// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/Abilities/RPGGameplayAbility.h"

#include "RPGGameplayAbility_FromEquipment.generated.h"

class URPGEquipmentInstance;
class URPGInventoryItemInstance;

/**
 * ULyraGameplayAbility_FromEquipment
 *
 * An ability granted by and associated with an equipment instance
 */
UCLASS()
class URPGGameplayAbility_FromEquipment : public URPGGameplayAbility
{
	GENERATED_BODY()

public:

	URPGGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "RPG|Ability")
	URPGEquipmentInstance* GetAssociatedEquipment() const;

	UFUNCTION(BlueprintCallable, Category = "RPG|Ability")
	URPGInventoryItemInstance* GetAssociatedItem() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

};
