// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "RPGGameData.generated.h"

#define UE_API RPGGAME_API

class UGameplayEffect;
class UObject;

/**
 * ULyraGameData
 *
 *	Non-mutable data asset that contains global game data.
 */
UCLASS(MinimalAPI, BlueprintType, Const, Meta = (DisplayName = "RPG Game Data", ShortTooltip = "Data asset containing global game data."))
class URPGGameData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UE_API URPGGameData();

	// Returns the loaded game data.
	static UE_API const URPGGameData& Get();

public:

	// Gameplay effect used to apply damage.  Uses SetByCaller for the damage magnitude.
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects", meta = (DisplayName = "Damage Gameplay Effect (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> DamageGameplayEffect_SetByCaller;

	// Gameplay effect used to apply healing.  Uses SetByCaller for the healing magnitude.
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects", meta = (DisplayName = "Heal Gameplay Effect (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> HealGameplayEffect_SetByCaller;

	// Gameplay effect used to add and remove dynamic tags.
	UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effects")
	TSoftClassPtr<UGameplayEffect> DynamicTagGameplayEffect;
};

#undef UE_API
