// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGGameplayCueManager.h"
#include "System/RPGLogChannels.h"
#include "AbilitySystemGlobals.h"

URPGGameplayCueManager* URPGGameplayCueManager::Get()
{
	if (UGameplayCueManager* GCM = UAbilitySystemGlobals::Get().GetGameplayCueManager())
	{
		return Cast<URPGGameplayCueManager>(GCM);
	}

	UE_LOG(LogRPG, Warning, TEXT("URPGGameplayCueManager::Get() - UGameplayCueManager singleton not yet available."));
	return nullptr;
}

void URPGGameplayCueManager::LoadAlwaysLoadedCues()
{
	// P0 存根实现，S34 集成时替换为完整的 Cue 加载逻辑
	UE_LOG(LogRPG, Log, TEXT("URPGGameplayCueManager::LoadAlwaysLoadedCues() - Stub called, no cues loaded."));
}
