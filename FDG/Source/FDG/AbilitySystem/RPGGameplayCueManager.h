// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayCueManager.h"
#include "RPGGameplayCueManager.generated.h"

/**
 * URPGGameplayCueManager
 *
 *	P0 存根实现，仅提供 AssetManager 编译所需的最小接口。
 *	S34 (GameplayCue系统) 集成时替换为完整实现。
 */
UCLASS()
class RPGGAME_API URPGGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()

public:

	static URPGGameplayCueManager* Get();

	void LoadAlwaysLoadedCues();
};
