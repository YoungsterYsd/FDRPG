// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSettingAction.h"
#include "GameSettingValueScalarDynamic.h"

#include "RPGSettingAction_SafeZoneEditor.generated.h"

class UGameSetting;
class UObject;


UCLASS()
class URPGSettingValueScalarDynamic_SafeZoneValue : public UGameSettingValueScalarDynamic
{
	GENERATED_BODY()

public:
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;
};

UCLASS()
class URPGSettingAction_SafeZoneEditor : public UGameSettingAction
{
	GENERATED_BODY()
	
public:
	URPGSettingAction_SafeZoneEditor();
	virtual TArray<UGameSetting*> GetChildSettings() override;

private:
	UPROPERTY()
	TObjectPtr<URPGSettingValueScalarDynamic_SafeZoneValue> SafeZoneValueSetting;
};
