// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "RPGPawnData.generated.h"

/**
 * URPGPawnData
 *
 *	P0 最小存根实现，仅提供 AssetManager 编译所需的基本类型。
 *	S01 (模块化角色框架) 集成时替换为完整实现，包含：
 *	- PawnClass, AbilitySets, InputConfig, DefaultCameraMode 等属性
 *	- TagRelationshipMapping
 */
UCLASS(Const, BlueprintType)
class RPGGAME_API URPGPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	URPGPawnData();
};
