// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "DataSource/GameSettingDataSourceDynamic.h" // IWYU pragma: keep
#include "GameSettingRegistry.h"
#include "Settings/RPGSettingsLocal.h" // IWYU pragma: keep

#include "RPGGameSettingRegistry.generated.h"

class ULocalPlayer;
class UObject;

//--------------------------------------
// ULyraGameSettingRegistry
//--------------------------------------

class UGameSettingCollection;
class URPGLocalPlayer;

DECLARE_LOG_CATEGORY_EXTERN(LogRPGGameSettingRegistry, Log, Log);

#define GET_SHARED_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName)							\
	MakeShared<FGameSettingDataSourceDynamic>(TArray<FString>({								\
		GET_FUNCTION_NAME_STRING_CHECKED(URPGLocalPlayer, GetSharedSettings),				\
		GET_FUNCTION_NAME_STRING_CHECKED(URPGSettingsShared, FunctionOrPropertyName)		\
	}))

#define GET_LOCAL_SETTINGS_FUNCTION_PATH(FunctionOrPropertyName)							\
	MakeShared<FGameSettingDataSourceDynamic>(TArray<FString>({								\
		GET_FUNCTION_NAME_STRING_CHECKED(URPGLocalPlayer, GetLocalSettings),				\
		GET_FUNCTION_NAME_STRING_CHECKED(URPGSettingsLocal, FunctionOrPropertyName)		\
	}))

/**
 * 
 */
UCLASS()
class URPGGameSettingRegistry : public UGameSettingRegistry
{
	GENERATED_BODY()

public:
	URPGGameSettingRegistry();

	static URPGGameSettingRegistry* Get(URPGLocalPlayer* InLocalPlayer);
	
	virtual void SaveChanges() override;

protected:
	virtual void OnInitialize(ULocalPlayer* InLocalPlayer) override;
	virtual bool IsFinishedInitializing() const override;

	UGameSettingCollection* InitializeVideoSettings(URPGLocalPlayer* InLocalPlayer);
	void InitializeVideoSettings_FrameRates(UGameSettingCollection* Screen, URPGLocalPlayer* InLocalPlayer);
	void AddPerformanceStatPage(UGameSettingCollection* Screen, URPGLocalPlayer* InLocalPlayer);

	UGameSettingCollection* InitializeAudioSettings(URPGLocalPlayer* InLocalPlayer);
	UGameSettingCollection* InitializeGameplaySettings(URPGLocalPlayer* InLocalPlayer);

	UGameSettingCollection* InitializeMouseAndKeyboardSettings(URPGLocalPlayer* InLocalPlayer);
	UGameSettingCollection* InitializeGamepadSettings(URPGLocalPlayer* InLocalPlayer);

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> VideoSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> AudioSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> GameplaySettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> MouseAndKeyboardSettings;

	UPROPERTY()
	TObjectPtr<UGameSettingCollection> GamepadSettings;
};
