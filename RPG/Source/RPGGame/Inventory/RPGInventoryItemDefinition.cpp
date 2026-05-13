// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGInventoryItemDefinition.h"

#include "Templates/SubclassOf.h"
#include "UObject/ObjectPtr.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGInventoryItemDefinition)

//////////////////////////////////////////////////////////////////////
// ULyraInventoryItemDefinition

URPGInventoryItemDefinition::URPGInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const URPGInventoryItemFragment* URPGInventoryItemDefinition::FindFragmentByClass(TSubclassOf<URPGInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (URPGInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////
// ULyraInventoryItemDefinition

const URPGInventoryItemFragment* URPGInventoryFunctionLibrary::FindItemDefinitionFragment(TSubclassOf<URPGInventoryItemDefinition> ItemDef, TSubclassOf<URPGInventoryItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<URPGInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}

