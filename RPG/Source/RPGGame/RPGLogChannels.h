// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

RPGGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogLyra, Log, All);
RPGGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogRPGExperience, Log, All);
RPGGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogRPGAbilitySystem, Log, All);
RPGGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogRPGTeams, Log, All);

RPGGAME_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
