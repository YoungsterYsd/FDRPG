// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "RPGPawnData.generated.h"

class URPGAbilitySet;
class URPGInputConfig;
class URPGCameraMode;
class URPGAbilityTagRelationshipMapping;
class UInputMappingContext;

/**
 * URPGPawnData
 *
 *	Pawn 配置数据资产，定义角色的能力集、输入配置、相机模式等。
 *	每种角色类型对应一个 PawnData 实例（必须用 Data Asset 方式创建）。
 */
UCLASS(BlueprintType, Meta = (DisplayName = "RPG Pawn Data", ShortTooltip = "Data asset defining a pawn's abilities, input, and camera configuration."))
class RPGGAME_API URPGPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	URPGPawnData();

	// Default pawn class to spawn for this pawn data
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	TSoftClassPtr<APawn> PawnClass;

	// Ability sets to grant when this pawn is possessed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSoftObjectPtr<URPGAbilitySet>> AbilitySets;

	// Input configuration used by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TSoftObjectPtr<URPGInputConfig> InputConfig;

	// Input mapping context to push when this pawn is possessed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> InputMappingContext;

	// Default camera mode to use when this pawn is possessed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TSoftClassPtr<URPGCameraMode> DefaultCameraMode;

	// Tag relationship mapping for ability activation rules (blocking/canceling)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	URPGAbilityTagRelationshipMapping* TagRelationshipMapping = nullptr;
};
