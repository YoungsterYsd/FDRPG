// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayModMagnitudeCalculation.h"

#include "RPGMMC_HealthMax.generated.h"

class UObject;

/**
 * URPGMMC_HealthMax
 *
 * 派生公式：HealthMax = HealthBase × (1 + HealthMul) + HealthBonus
 * 读 URPGPrimaryAttributeSet 三个字段，输出写入 URPGHealthSet.HealthMax（由 GE_Health_Derive_Max 驱动）。
 *
 * 使用：在 GE_Health_Derive_Max 的 Modifier 上设：
 *   - Attribute                     = RPGHealthSet.HealthMax
 *   - Modifier Op                   = Override
 *   - Magnitude Calculation Type    = Custom Calculation Class
 *   - Calculation Class             = RPGMMC_HealthMax
 */
UCLASS()
class URPGMMC_HealthMax : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:

	URPGMMC_HealthMax();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:

	FGameplayEffectAttributeCaptureDefinition HealthBaseDef;
	FGameplayEffectAttributeCaptureDefinition HealthBonusDef;
	FGameplayEffectAttributeCaptureDefinition HealthMulDef;
};
