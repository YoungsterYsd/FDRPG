// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGGameEngine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGGameEngine)

class IEngineLoop;


URPGGameEngine::URPGGameEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URPGGameEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);
}

