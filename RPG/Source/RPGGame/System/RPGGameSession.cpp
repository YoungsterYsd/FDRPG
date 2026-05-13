// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGGameSession.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGGameSession)


ARPGGameSession::ARPGGameSession(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool ARPGGameSession::ProcessAutoLogin()
{
	// This is actually handled in LyraGameMode::TryDedicatedServerLogin
	return true;
}

void ARPGGameSession::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void ARPGGameSession::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
}

