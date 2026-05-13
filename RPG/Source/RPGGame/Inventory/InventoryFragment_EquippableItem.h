// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Inventory/RPGInventoryItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "InventoryFragment_EquippableItem.generated.h"

class URPGEquipmentDefinition;
class UObject;

UCLASS()
class UInventoryFragment_EquippableItem : public URPGInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=RPG)
	TSubclassOf<URPGEquipmentDefinition> EquipmentDefinition;
};
