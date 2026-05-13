// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "IndicatorLibrary.generated.h"

#define UE_API RPGGAME_API

class AController;
class URPGIndicatorManagerComponent;
class UObject;
struct FFrame;

UCLASS(MinimalAPI)
class UIndicatorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UE_API UIndicatorLibrary();
	
	/**  */
	UFUNCTION(BlueprintCallable, Category = Indicator)
	static UE_API URPGIndicatorManagerComponent* GetIndicatorManagerComponent(AController* Controller);
};

#undef UE_API
