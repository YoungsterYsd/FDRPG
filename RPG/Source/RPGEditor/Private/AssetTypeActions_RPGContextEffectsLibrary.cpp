// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTypeActions_RPGContextEffectsLibrary.h"

#include "Feedback/ContextEffects/RPGContextEffectsLibrary.h"

class UClass;

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_RPGContextEffectsLibrary::GetSupportedClass() const
{
	return URPGContextEffectsLibrary::StaticClass();
}

#undef LOCTEXT_NAMESPACE
