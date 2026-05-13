// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGWidgetFactory.h"
#include "Templates/SubclassOf.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGWidgetFactory)

class UUserWidget;

TSubclassOf<UUserWidget> URPGWidgetFactory::FindWidgetClassForData_Implementation(const UObject* Data) const
{
	return TSubclassOf<UUserWidget>();
}
