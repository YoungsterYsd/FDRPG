// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGPerformanceSettings.h"

#include "Engine/PlatformSettingsManager.h"
#include "Misc/EnumRange.h"
#include "Performance/RPGPerformanceStatTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGPerformanceSettings)

//////////////////////////////////////////////////////////////////////

URPGPlatformSpecificRenderingSettings::URPGPlatformSpecificRenderingSettings()
{
	MobileFrameRateLimits.Append({ 20, 30, 45, 60, 90, 120 });
}

const URPGPlatformSpecificRenderingSettings* URPGPlatformSpecificRenderingSettings::Get()
{
	URPGPlatformSpecificRenderingSettings* Result = UPlatformSettingsManager::Get().GetSettingsForPlatform<ThisClass>();
	check(Result);
	return Result;
}

//////////////////////////////////////////////////////////////////////

URPGPerformanceSettings::URPGPerformanceSettings()
{
	PerPlatformSettings.Initialize(URPGPlatformSpecificRenderingSettings::StaticClass());

	CategoryName = TEXT("Game");

	DesktopFrameRateLimits.Append({ 30, 60, 120, 144, 160, 165, 180, 200, 240, 360 });

	// Default to all stats are allowed
	FRPGPerformanceStatGroup& StatGroup = UserFacingPerformanceStats.AddDefaulted_GetRef();
	for (ERPGDisplayablePerformanceStat PerfStat : TEnumRange<ERPGDisplayablePerformanceStat>())
	{
		StatGroup.AllowedStats.Add(PerfStat);
	}
}

