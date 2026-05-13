// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGSettingValueDiscrete_PerfStat.h"

#include "CommonUIVisibilitySubsystem.h"
#include "Performance/RPGPerformanceSettings.h"
#include "Performance/RPGPerformanceStatTypes.h"
#include "Settings/RPGSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGSettingValueDiscrete_PerfStat)

class ULocalPlayer;

#define LOCTEXT_NAMESPACE "LyraSettings"

//////////////////////////////////////////////////////////////////////

class FGameSettingEditCondition_PerfStatAllowed : public FGameSettingEditCondition
{
public:
	static TSharedRef<FGameSettingEditCondition_PerfStatAllowed> Get(ERPGDisplayablePerformanceStat Stat)
	{
		return MakeShared<FGameSettingEditCondition_PerfStatAllowed>(Stat);
	}

	FGameSettingEditCondition_PerfStatAllowed(ERPGDisplayablePerformanceStat Stat)
		: AssociatedStat(Stat)
	{
	}

	//~FGameSettingEditCondition interface
	virtual void GatherEditState(const ULocalPlayer* InLocalPlayer, FGameSettingEditableState& InOutEditState) const override
	{
		const FGameplayTagContainer& VisibilityTags = UCommonUIVisibilitySubsystem::GetChecked(InLocalPlayer)->GetVisibilityTags();

		bool bCanShowStat = false;
		for (const FRPGPerformanceStatGroup& Group : GetDefault<URPGPerformanceSettings>()->UserFacingPerformanceStats) //@TODO: Move this stuff to per-platform instead of doing vis queries too?
		{
			if (Group.AllowedStats.Contains(AssociatedStat))
			{
				const bool bShowGroup = (Group.VisibilityQuery.IsEmpty() || Group.VisibilityQuery.Matches(VisibilityTags));
				if (bShowGroup)
				{
					bCanShowStat = true;
					break;
				}
			}
		}

		if (!bCanShowStat)
		{
			InOutEditState.Hide(TEXT("Stat is not listed in ULyraPerformanceSettings or is suppressed by current platform traits"));
		}
	}
	//~End of FGameSettingEditCondition interface

private:
	ERPGDisplayablePerformanceStat AssociatedStat;
};

//////////////////////////////////////////////////////////////////////

URPGSettingValueDiscrete_PerfStat::URPGSettingValueDiscrete_PerfStat()
{
}

void URPGSettingValueDiscrete_PerfStat::SetStat(ERPGDisplayablePerformanceStat InStat)
{
	StatToDisplay = InStat;
	SetDevName(FName(*FString::Printf(TEXT("PerfStat_%d"), (int32)StatToDisplay)));
	AddEditCondition(FGameSettingEditCondition_PerfStatAllowed::Get(StatToDisplay));
}

void URPGSettingValueDiscrete_PerfStat::AddMode(FText&& Label, ERPGStatDisplayMode Mode)
{
	Options.Emplace(MoveTemp(Label));
	DisplayModes.Add(Mode);
}

void URPGSettingValueDiscrete_PerfStat::OnInitialized()
{
	Super::OnInitialized();

	AddMode(LOCTEXT("PerfStatDisplayMode_None", "None"), ERPGStatDisplayMode::Hidden);
	AddMode(LOCTEXT("PerfStatDisplayMode_TextOnly", "Text Only"), ERPGStatDisplayMode::TextOnly);
	AddMode(LOCTEXT("PerfStatDisplayMode_GraphOnly", "Graph Only"), ERPGStatDisplayMode::GraphOnly);
	AddMode(LOCTEXT("PerfStatDisplayMode_TextAndGraph", "Text and Graph"), ERPGStatDisplayMode::TextAndGraph);
}

void URPGSettingValueDiscrete_PerfStat::StoreInitial()
{
	const URPGSettingsLocal* Settings = URPGSettingsLocal::Get();
	InitialMode = Settings->GetPerfStatDisplayState(StatToDisplay);
}

void URPGSettingValueDiscrete_PerfStat::ResetToDefault()
{
	URPGSettingsLocal* Settings = URPGSettingsLocal::Get();
	Settings->SetPerfStatDisplayState(StatToDisplay, ERPGStatDisplayMode::Hidden);
	NotifySettingChanged(EGameSettingChangeReason::ResetToDefault);
}

void URPGSettingValueDiscrete_PerfStat::RestoreToInitial()
{
	URPGSettingsLocal* Settings = URPGSettingsLocal::Get();
	Settings->SetPerfStatDisplayState(StatToDisplay, InitialMode);
	NotifySettingChanged(EGameSettingChangeReason::RestoreToInitial);
}

void URPGSettingValueDiscrete_PerfStat::SetDiscreteOptionByIndex(int32 Index)
{
	if (DisplayModes.IsValidIndex(Index))
	{
		const ERPGStatDisplayMode DisplayMode = DisplayModes[Index];

		URPGSettingsLocal* Settings = URPGSettingsLocal::Get();
		Settings->SetPerfStatDisplayState(StatToDisplay, DisplayMode);
	}
	NotifySettingChanged(EGameSettingChangeReason::Change);
}

int32 URPGSettingValueDiscrete_PerfStat::GetDiscreteOptionIndex() const
{
	const URPGSettingsLocal* Settings = URPGSettingsLocal::Get();
	return DisplayModes.Find(Settings->GetPerfStatDisplayState(StatToDisplay));
}

TArray<FText> URPGSettingValueDiscrete_PerfStat::GetDiscreteOptions() const
{
	return Options;
}

#undef LOCTEXT_NAMESPACE
