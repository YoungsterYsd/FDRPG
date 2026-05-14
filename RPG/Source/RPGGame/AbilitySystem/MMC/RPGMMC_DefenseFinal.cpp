// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGMMC_DefenseFinal.h"
#include "AbilitySystem/Attributes/RPGPrimaryAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGMMC_DefenseFinal)


URPGMMC_DefenseFinal::URPGMMC_DefenseFinal()
{
	DefenseBaseDef  = FGameplayEffectAttributeCaptureDefinition(URPGPrimaryAttributeSet::GetDefenseBaseAttribute(),  EGameplayEffectAttributeCaptureSource::Target, false);
	DefenseBonusDef = FGameplayEffectAttributeCaptureDefinition(URPGPrimaryAttributeSet::GetDefenseBonusAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	DefenseMulDef   = FGameplayEffectAttributeCaptureDefinition(URPGPrimaryAttributeSet::GetDefenseMulAttribute(),   EGameplayEffectAttributeCaptureSource::Target, false);

	RelevantAttributesToCapture.Add(DefenseBaseDef);
	RelevantAttributesToCapture.Add(DefenseBonusDef);
	RelevantAttributesToCapture.Add(DefenseMulDef);
}

float URPGMMC_DefenseFinal::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = SourceTags;
	EvalParams.TargetTags = TargetTags;

	float Base  = 0.f;
	float Bonus = 0.f;
	float Mul   = 0.f;

	GetCapturedAttributeMagnitude(DefenseBaseDef,  Spec, EvalParams, Base);
	GetCapturedAttributeMagnitude(DefenseBonusDef, Spec, EvalParams, Bonus);
	GetCapturedAttributeMagnitude(DefenseMulDef,   Spec, EvalParams, Mul);

	return Base * (1.f + Mul) + Bonus;
}
