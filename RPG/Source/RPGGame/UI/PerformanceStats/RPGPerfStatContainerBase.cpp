// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGPerfStatContainerBase.h"

#include "Blueprint/WidgetTree.h"
#include "RPGPerfStatWidgetBase.h"
#include "Settings/RPGSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGPerfStatContainerBase)

//////////////////////////////////////////////////////////////////////
// ULyraPerfStatsContainerBase

URPGPerfStatContainerBase::URPGPerfStatContainerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URPGPerfStatContainerBase::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateVisibilityOfChildren();

	URPGSettingsLocal::Get()->OnPerfStatDisplayStateChanged().AddUObject(this, &ThisClass::UpdateVisibilityOfChildren);
}

void URPGPerfStatContainerBase::NativeDestruct()
{
	URPGSettingsLocal::Get()->OnPerfStatDisplayStateChanged().RemoveAll(this);

	Super::NativeDestruct();
}

void URPGPerfStatContainerBase::UpdateVisibilityOfChildren()
{
	URPGSettingsLocal* UserSettings = URPGSettingsLocal::Get();

	const bool bShowTextWidgets = (StatDisplayModeFilter == ERPGStatDisplayMode::TextOnly) || (StatDisplayModeFilter == ERPGStatDisplayMode::TextAndGraph);
	const bool bShowGraphWidgets = (StatDisplayModeFilter == ERPGStatDisplayMode::GraphOnly) || (StatDisplayModeFilter == ERPGStatDisplayMode::TextAndGraph);
	
	check(WidgetTree);
	WidgetTree->ForEachWidget([&](UWidget* Widget)
	{
		if (URPGPerfStatWidgetBase* TypedWidget = Cast<URPGPerfStatWidgetBase>(Widget))
		{
			const ERPGStatDisplayMode SettingMode = UserSettings->GetPerfStatDisplayState(TypedWidget->GetStatToDisplay());

			bool bShowWidget = false;
			switch (SettingMode)
			{
			case ERPGStatDisplayMode::Hidden:
				bShowWidget = false;
				break;
			case ERPGStatDisplayMode::TextOnly:
				bShowWidget = bShowTextWidgets;
				break;
			case ERPGStatDisplayMode::GraphOnly:
				bShowWidget = bShowGraphWidgets;
				break;
			case ERPGStatDisplayMode::TextAndGraph:
				bShowWidget = bShowTextWidgets || bShowGraphWidgets;
				break;
			}

			TypedWidget->SetVisibility(bShowWidget ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		}
	});
}

