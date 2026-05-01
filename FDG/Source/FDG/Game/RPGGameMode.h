// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RPGGameMode.generated.h"

/**
 * ARPGGameMode
 *
 *	RPG 游戏 GameMode，配置默认 Pawn、PlayerController、PlayerState 等类。
 *	具体类映射在 BP_RPGGameMode 蓝图中设置。
 */
UCLASS(Meta = (ShortTooltip = "Game mode for the RPG game."))
class RPGGAME_API ARPGGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ARPGGameMode();
};
