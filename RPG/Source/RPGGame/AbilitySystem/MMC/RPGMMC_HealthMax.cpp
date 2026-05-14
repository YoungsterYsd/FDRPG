// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGMMC_HealthMax.h"
#include "AbilitySystem/Attributes/RPGPrimaryAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGMMC_HealthMax)


URPGMMC_HealthMax::URPGMMC_HealthMax()
{
	HealthBaseDef  = FGameplayEffectAttributeCaptureDefinition(URPGPrimaryAttributeSet::GetHealthBaseAttribute(),  EGameplayEffectAttributeCaptureSource::Target, false);
	HealthBonusDef = FGameplayEffectAttributeCaptureDefinition(URPGPrimaryAttributeSet::GetHealthBonusAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	HealthMulDef   = FGameplayEffectAttributeCaptureDefinition(URPGPrimaryAttributeSet::GetHealthMulAttribute(),   EGameplayEffectAttributeCaptureSource::Target, false);

	RelevantAttributesToCapture.Add(HealthBaseDef);
	RelevantAttributesToCapture.Add(HealthBonusDef);
	RelevantAttributesToCapture.Add(HealthMulDef);
}

float URPGMMC_HealthMax::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = SourceTags;
	EvalParams.TargetTags = TargetTags;

	float Base  = 0.f;
	float Bonus = 0.f;
	float Mul   = 0.f;

	GetCapturedAttributeMagnitude(HealthBaseDef,  Spec, EvalParams, Base);
	GetCapturedAttributeMagnitude(HealthBonusDef, Spec, EvalParams, Bonus);
	GetCapturedAttributeMagnitude(HealthMulDef,   Spec, EvalParams, Mul);

	return Base * (1.f + Mul) + Bonus;
}
