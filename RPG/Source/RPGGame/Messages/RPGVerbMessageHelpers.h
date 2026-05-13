// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "RPGVerbMessageHelpers.generated.h"

#define UE_API RPGGAME_API

struct FGameplayCueParameters;
struct FRPGVerbMessage;

class APlayerController;
class APlayerState;
class UObject;
struct FFrame;


UCLASS(MinimalAPI)
class URPGVerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "RPG")
	static UE_API APlayerState* GetPlayerStateFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "RPG")
	static UE_API APlayerController* GetPlayerControllerFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "RPG")
	static UE_API FGameplayCueParameters VerbMessageToCueParameters(const FRPGVerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "RPG")
	static UE_API FRPGVerbMessage CueParametersToVerbMessage(const FGameplayCueParameters& Params);
};

#undef UE_API
