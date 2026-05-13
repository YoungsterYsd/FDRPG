// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGAimSensitivityData.h"

#include "Settings/RPGSettingsShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGAimSensitivityData)

URPGAimSensitivityData::URPGAimSensitivityData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SensitivityMap =
	{
		{ ERPGGamepadSensitivity::Slow,			0.5f },
		{ ERPGGamepadSensitivity::SlowPlus,		0.75f },
		{ ERPGGamepadSensitivity::SlowPlusPlus,	0.9f },
		{ ERPGGamepadSensitivity::Normal,		1.0f },
		{ ERPGGamepadSensitivity::NormalPlus,	1.1f },
		{ ERPGGamepadSensitivity::NormalPlusPlus,1.25f },
		{ ERPGGamepadSensitivity::Fast,			1.5f },
		{ ERPGGamepadSensitivity::FastPlus,		1.75f },
		{ ERPGGamepadSensitivity::FastPlusPlus,	2.0f },
		{ ERPGGamepadSensitivity::Insane,		2.5f },
	};
}

const float URPGAimSensitivityData::SensitivtyEnumToFloat(const ERPGGamepadSensitivity InSensitivity) const
{
	if (const float* Sens = SensitivityMap.Find(InSensitivity))
	{
		return *Sens;
	}

	return 1.0f;
}

