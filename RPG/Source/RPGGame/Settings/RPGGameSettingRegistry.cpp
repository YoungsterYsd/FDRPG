// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGGameSettingRegistry.h"

#include "GameSettingCollection.h"
#include "RPGSettingsLocal.h"
#include "RPGSettingsShared.h"
#include "Player/RPGLocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGGameSettingRegistry)

DEFINE_LOG_CATEGORY(LogRPGGameSettingRegistry);

#define LOCTEXT_NAMESPACE "Lyra"

//--------------------------------------
// ULyraGameSettingRegistry
//--------------------------------------

URPGGameSettingRegistry::URPGGameSettingRegistry()
{
}

URPGGameSettingRegistry* URPGGameSettingRegistry::Get(URPGLocalPlayer* InLocalPlayer)
{
	URPGGameSettingRegistry* Registry = FindObject<URPGGameSettingRegistry>(InLocalPlayer, TEXT("LyraGameSettingRegistry"), EFindObjectFlags::ExactClass);
	if (Registry == nullptr)
	{
		Registry = NewObject<URPGGameSettingRegistry>(InLocalPlayer, TEXT("LyraGameSettingRegistry"));
		Registry->Initialize(InLocalPlayer);
	}

	return Registry;
}

bool URPGGameSettingRegistry::IsFinishedInitializing() const
{
	if (Super::IsFinishedInitializing())
	{
		if (URPGLocalPlayer* LocalPlayer = Cast<URPGLocalPlayer>(OwningLocalPlayer))
		{
			if (LocalPlayer->GetSharedSettings() == nullptr)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

void URPGGameSettingRegistry::OnInitialize(ULocalPlayer* InLocalPlayer)
{
	URPGLocalPlayer* RPGLocalPlayer = Cast<URPGLocalPlayer>(InLocalPlayer);

	VideoSettings = InitializeVideoSettings(RPGLocalPlayer);
	InitializeVideoSettings_FrameRates(VideoSettings, RPGLocalPlayer);
	RegisterSetting(VideoSettings);

	AudioSettings = InitializeAudioSettings(RPGLocalPlayer);
	RegisterSetting(AudioSettings);

	GameplaySettings = InitializeGameplaySettings(RPGLocalPlayer);
	RegisterSetting(GameplaySettings);

	MouseAndKeyboardSettings = InitializeMouseAndKeyboardSettings(RPGLocalPlayer);
	RegisterSetting(MouseAndKeyboardSettings);

	GamepadSettings = InitializeGamepadSettings(RPGLocalPlayer);
	RegisterSetting(GamepadSettings);
}

void URPGGameSettingRegistry::SaveChanges()
{
	Super::SaveChanges();
	
	if (URPGLocalPlayer* LocalPlayer = Cast<URPGLocalPlayer>(OwningLocalPlayer))
	{
		// Game user settings need to be applied to handle things like resolution, this saves indirectly
		LocalPlayer->GetLocalSettings()->ApplySettings(false);
		
		LocalPlayer->GetSharedSettings()->ApplySettings();
		LocalPlayer->GetSharedSettings()->SaveSettings();
	}
}

#undef LOCTEXT_NAMESPACE

