// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "RPGAimSensitivityData.generated.h"

#define UE_API RPGGAME_API

enum class ERPGGamepadSensitivity : uint8;

class UObject;

/** Defines a set of gamepad sensitivity to a float value. */
UCLASS(MinimalAPI, BlueprintType, Const, Meta = (DisplayName = "RPG Aim Sensitivity Data", ShortTooltip = "Data asset used to define a map of Gamepad Sensitivty to a float value."))
class URPGAimSensitivityData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UE_API URPGAimSensitivityData(const FObjectInitializer& ObjectInitializer);
	
	UE_API const float SensitivtyEnumToFloat(const ERPGGamepadSensitivity InSensitivity) const;
	
protected:
	/** Map of SensitivityMap settings to their corresponding float */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ERPGGamepadSensitivity, float> SensitivityMap;
};

#undef UE_API
