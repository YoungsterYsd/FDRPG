// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "RPGPawnData.generated.h"

#define UE_API RPGGAME_API

class APawn;
class URPGAbilitySet;
class URPGAbilityTagRelationshipMapping;
class URPGCameraMode;
class URPGInputConfig;
class UObject;


/**
 * ULyraPawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(MinimalAPI, BlueprintType, Const, Meta = (DisplayName = "RPG Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class URPGPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UE_API URPGPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// Class to instantiate for this pawn (should usually derive from ALyraPawn or ALyraCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Abilities")
	TArray<TObjectPtr<URPGAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Abilities")
	TObjectPtr<URPGAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Input")
	TObjectPtr<URPGInputConfig> InputConfig;

	// Default camera mode used by player controlled pawns.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Camera")
	TSubclassOf<URPGCameraMode> DefaultCameraMode;
};

#undef UE_API
