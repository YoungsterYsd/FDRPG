// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Inventory/RPGInventoryItemDefinition.h"

#include "InventoryFragment_ReticleConfig.generated.h"

class URPGReticleWidgetBase;
class UObject;

UCLASS()
class UInventoryFragment_ReticleConfig : public URPGInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Reticle)
	TArray<TSubclassOf<URPGReticleWidgetBase>> ReticleWidgets;
};
