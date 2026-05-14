// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayModMagnitudeCalculation.h"

#include "RPGMMC_AttackFinal.generated.h"

class UObject;

/**
 * URPGMMC_AttackFinal
 *
 * 派生公式：AttackFinal = AttackBase × (1 + AttackMul) + AttackBonus
 * 读 URPGPrimaryAttributeSet 三个字段，输出写入 URPGPrimaryAttributeSet.AttackFinal（由 GE_Attack_Derive_Final 驱动）。
 */
UCLASS()
class URPGMMC_AttackFinal : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:

	URPGMMC_AttackFinal();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:

	FGameplayEffectAttributeCaptureDefinition AttackBaseDef;
	FGameplayEffectAttributeCaptureDefinition AttackBonusDef;
	FGameplayEffectAttributeCaptureDefinition AttackMulDef;
};
