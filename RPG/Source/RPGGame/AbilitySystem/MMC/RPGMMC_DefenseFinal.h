// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayModMagnitudeCalculation.h"

#include "RPGMMC_DefenseFinal.generated.h"

class UObject;

/**
 * URPGMMC_DefenseFinal
 *
 * 派生公式：DefenseFinal = DefenseBase × (1 + DefenseMul) + DefenseBonus
 * 读 URPGPrimaryAttributeSet 三个字段，输出写入 URPGPrimaryAttributeSet.DefenseFinal（由 GE_Defense_Derive_Final 驱动）。
 */
UCLASS()
class URPGMMC_DefenseFinal : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:

	URPGMMC_DefenseFinal();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:

	FGameplayEffectAttributeCaptureDefinition DefenseBaseDef;
	FGameplayEffectAttributeCaptureDefinition DefenseBonusDef;
	FGameplayEffectAttributeCaptureDefinition DefenseMulDef;
};
