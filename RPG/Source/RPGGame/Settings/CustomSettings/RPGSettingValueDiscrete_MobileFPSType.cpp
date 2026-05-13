// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGSettingValueDiscrete_MobileFPSType.h"

#include "Performance/RPGPerformanceSettings.h"
#include "Settings/RPGSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGSettingValueDiscrete_MobileFPSType)

#define LOCTEXT_NAMESPACE "LyraSettings"

URPGSettingValueDiscrete_MobileFPSType::URPGSettingValueDiscrete_MobileFPSType()
{
}

void URPGSettingValueDiscrete_MobileFPSType::OnInitialized()
{
	Super::OnInitialized();

	const URPGPlatformSpecificRenderingSettings* PlatformSettings = URPGPlatformSpecificRenderingSettings::Get();
	const URPGSettingsLocal* UserSettings = URPGSettingsLocal::Get();

	for (int32 TestLimit : PlatformSettings->MobileFrameRateLimits)
	{
		if (URPGSettingsLocal::IsSupportedMobileFramePace(TestLimit))
		{
			FPSOptions.Add(TestLimit, MakeLimitString(TestLimit));
		}
	}

	const int32 FirstFrameRateWithQualityLimit = UserSettings->GetFirstFrameRateWithQualityLimit();
	if (FirstFrameRateWithQualityLimit > 0)
	{
		SetWarningRichText(FText::Format(LOCTEXT("MobileFPSType_Note", "<strong>Note: Changing the framerate setting to {0} or higher might lower your Quality Presets.</>"), MakeLimitString(FirstFrameRateWithQualityLimit)));
	}
}

int32 URPGSettingValueDiscrete_MobileFPSType::GetDefaultFPS() const
{
	return URPGSettingsLocal::GetDefaultMobileFrameRate();
}

FText URPGSettingValueDiscrete_MobileFPSType::MakeLimitString(int32 Number)
{
	return FText::Format(LOCTEXT("MobileFrameRateOption", "{0} FPS"), FText::AsNumber(Number));
}

void URPGSettingValueDiscrete_MobileFPSType::StoreInitial()
{
	InitialValue = GetValue();
}

void URPGSettingValueDiscrete_MobileFPSType::ResetToDefault()
{
	SetValue(GetDefaultFPS(), EGameSettingChangeReason::ResetToDefault);
}

void URPGSettingValueDiscrete_MobileFPSType::RestoreToInitial()
{
	SetValue(InitialValue, EGameSettingChangeReason::RestoreToInitial);
}

void URPGSettingValueDiscrete_MobileFPSType::SetDiscreteOptionByIndex(int32 Index)
{
	TArray<int32> FPSOptionsModes;
	FPSOptions.GenerateKeyArray(FPSOptionsModes);

	int32 NewMode = FPSOptionsModes.IsValidIndex(Index) ? FPSOptionsModes[Index] : GetDefaultFPS();

	SetValue(NewMode, EGameSettingChangeReason::Change);
}

int32 URPGSettingValueDiscrete_MobileFPSType::GetDiscreteOptionIndex() const
{
	TArray<int32> FPSOptionsModes;
	FPSOptions.GenerateKeyArray(FPSOptionsModes);
	return FPSOptionsModes.IndexOfByKey(GetValue());
}

TArray<FText> URPGSettingValueDiscrete_MobileFPSType::GetDiscreteOptions() const
{
	TArray<FText> Options;
	FPSOptions.GenerateValueArray(Options);

	return Options;
}

int32 URPGSettingValueDiscrete_MobileFPSType::GetValue() const
{
	return URPGSettingsLocal::Get()->GetDesiredMobileFrameRateLimit();
}

void URPGSettingValueDiscrete_MobileFPSType::SetValue(int32 NewLimitFPS, EGameSettingChangeReason InReason)
{
	URPGSettingsLocal::Get()->SetDesiredMobileFrameRateLimit(NewLimitFPS);

	NotifySettingChanged(InReason);
}

#undef LOCTEXT_NAMESPACE

