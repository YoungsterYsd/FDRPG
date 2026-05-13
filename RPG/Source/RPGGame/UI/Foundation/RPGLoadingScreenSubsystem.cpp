// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Foundation/RPGLoadingScreenSubsystem.h"

#include "Blueprint/UserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGLoadingScreenSubsystem)

class UUserWidget;

//////////////////////////////////////////////////////////////////////
// ULyraLoadingScreenSubsystem

URPGLoadingScreenSubsystem::URPGLoadingScreenSubsystem()
{
}

void URPGLoadingScreenSubsystem::SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (LoadingScreenWidgetClass != NewWidgetClass)
	{
		LoadingScreenWidgetClass = NewWidgetClass;

		OnLoadingScreenWidgetChanged.Broadcast(LoadingScreenWidgetClass);
	}
}

TSubclassOf<UUserWidget> URPGLoadingScreenSubsystem::GetLoadingScreenContentWidget() const
{
	return LoadingScreenWidgetClass;
}

