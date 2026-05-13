// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGSettingScreen.h"

#include "Input/CommonUIInputTypes.h"
#include "Player/RPGLocalPlayer.h"
#include "Settings/RPGGameSettingRegistry.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGSettingScreen)

class UGameSettingRegistry;

void URPGSettingScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BackHandle = RegisterUIActionBinding(FBindUIActionArgs(BackInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackAction)));
	ApplyHandle = RegisterUIActionBinding(FBindUIActionArgs(ApplyInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleApplyAction)));
	CancelChangesHandle = RegisterUIActionBinding(FBindUIActionArgs(CancelChangesInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleCancelChangesAction)));
}

UGameSettingRegistry* URPGSettingScreen::CreateRegistry()
{
	URPGGameSettingRegistry* NewRegistry = NewObject<URPGGameSettingRegistry>();

	if (URPGLocalPlayer* LocalPlayer = CastChecked<URPGLocalPlayer>(GetOwningLocalPlayer()))
	{
		NewRegistry->Initialize(LocalPlayer);
	}

	return NewRegistry;
}

void URPGSettingScreen::HandleBackAction()
{
	if (AttemptToPopNavigation())
	{
		return;
	}

	ApplyChanges();

	DeactivateWidget();
}

void URPGSettingScreen::HandleApplyAction()
{
	ApplyChanges();
}

void URPGSettingScreen::HandleCancelChangesAction()
{
	CancelChanges();
}

void URPGSettingScreen::OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty)
{
	if (bSettingsDirty)
	{
		if (!GetActionBindings().Contains(ApplyHandle))
		{
			AddActionBinding(ApplyHandle);
		}
		if (!GetActionBindings().Contains(CancelChangesHandle))
		{
			AddActionBinding(CancelChangesHandle);
		}
	}
	else
	{
		RemoveActionBinding(ApplyHandle);
		RemoveActionBinding(CancelChangesHandle);
	}
}
