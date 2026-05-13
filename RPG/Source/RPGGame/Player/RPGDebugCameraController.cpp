// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGDebugCameraController.h"
#include "RPGCheatManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGDebugCameraController)


ARPGDebugCameraController::ARPGDebugCameraController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Use the same cheat class as LyraPlayerController to allow toggling the debug camera through cheats.
	CheatClass = URPGCheatManager::StaticClass();
}

void ARPGDebugCameraController::AddCheats(bool bForce)
{
	// Mirrors LyraPlayerController's AddCheats() to avoid the player becoming stuck in the debug camera.
#if USING_CHEAT_MANAGER
	Super::AddCheats(true);
#else //#if USING_CHEAT_MANAGER
	Super::AddCheats(bForce);
#endif // #else //#if USING_CHEAT_MANAGER
}

