// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RPGWidgetFactory.h"
#include "Templates/SubclassOf.h"
#include "UObject/SoftObjectPtr.h"

#include "RPGWidgetFactory_Class.generated.h"

#define UE_API RPGGAME_API

class UObject;
class UUserWidget;

UCLASS(MinimalAPI)
class URPGWidgetFactory_Class : public URPGWidgetFactory
{
	GENERATED_BODY()

public:
	URPGWidgetFactory_Class() { }

	UE_API virtual TSubclassOf<UUserWidget> FindWidgetClassForData_Implementation(const UObject* Data) const override;
	
protected:
	UPROPERTY(EditAnywhere, Category = ListEntries, meta = (AllowAbstract))
	TMap<TSoftClassPtr<UObject>, TSubclassOf<UUserWidget>> EntryWidgetForClass;
};

#undef UE_API
