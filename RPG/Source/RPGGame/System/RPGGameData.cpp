// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGGameData.h"
#include "RPGAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGGameData)

URPGGameData::URPGGameData()
{
}

const URPGGameData& URPGGameData::URPGGameData::Get()
{
	return URPGAssetManager::Get().GetGameData();
}
