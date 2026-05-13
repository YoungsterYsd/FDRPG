// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ControllerComponent.h"

#include "RPGIndicatorManagerComponent.generated.h"

#define UE_API RPGGAME_API

class AController;
class UIndicatorDescriptor;
class UObject;
struct FFrame;

/**
 * @class ULyraIndicatorManagerComponent
 */
UCLASS(MinimalAPI, BlueprintType, Blueprintable)
class URPGIndicatorManagerComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	UE_API URPGIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer);

	static UE_API URPGIndicatorManagerComponent* GetComponent(AController* Controller);

	UFUNCTION(BlueprintCallable, Category = Indicator)
	UE_API void AddIndicator(UIndicatorDescriptor* IndicatorDescriptor);
	
	UFUNCTION(BlueprintCallable, Category = Indicator)
	UE_API void RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	DECLARE_EVENT_OneParam(URPGIndicatorManagerComponent, FIndicatorEvent, UIndicatorDescriptor* Descriptor)
	FIndicatorEvent OnIndicatorAdded;
	FIndicatorEvent OnIndicatorRemoved;

	const TArray<UIndicatorDescriptor*>& GetIndicators() const { return Indicators; }

private:
	UPROPERTY()
	TArray<TObjectPtr<UIndicatorDescriptor>> Indicators;
};

#undef UE_API
