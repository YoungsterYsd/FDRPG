// Copyright Epic Games, Inc. All Rights Reserved.

#include "IndicatorLibrary.h"

#include "RPGIndicatorManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IndicatorLibrary)

class AController;

UIndicatorLibrary::UIndicatorLibrary()
{
}

URPGIndicatorManagerComponent* UIndicatorLibrary::GetIndicatorManagerComponent(AController* Controller)
{
	return URPGIndicatorManagerComponent::GetComponent(Controller);
}

