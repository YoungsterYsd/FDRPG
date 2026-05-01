// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGGameMode.h"
#include "Player/RPGPlayerState.h"

ARPGGameMode::ARPGGameMode()
{
	// Default classes will be configured in BP_RPGGameMode blueprint
	// These are fallback C++ defaults
	PlayerStateClass = ARPGPlayerState::StaticClass();
}
