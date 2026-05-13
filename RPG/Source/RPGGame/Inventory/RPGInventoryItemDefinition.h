// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "RPGInventoryItemDefinition.generated.h"

template <typename T> class TSubclassOf;

class URPGInventoryItemInstance;
struct FFrame;

//////////////////////////////////////////////////////////////////////

// Represents a fragment of an item definition
UCLASS(MinimalAPI, DefaultToInstanced, EditInlineNew, Abstract)
class URPGInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(URPGInventoryItemInstance* Instance) const {}
};

//////////////////////////////////////////////////////////////////////

/**
 * ULyraInventoryItemDefinition
 */
UCLASS(Blueprintable, Const, Abstract)
class URPGInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	URPGInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<URPGInventoryItemFragment>> Fragments;

public:
	const URPGInventoryItemFragment* FindFragmentByClass(TSubclassOf<URPGInventoryItemFragment> FragmentClass) const;
};

//@TODO: Make into a subsystem instead?
UCLASS()
class URPGInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	static const URPGInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<URPGInventoryItemDefinition> ItemDef, TSubclassOf<URPGInventoryItemFragment> FragmentClass);
};
