// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGMMC_AttackFinal.h"
#include "AbilitySystem/Attributes/RPGPrimaryAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGMMC_AttackFinal)


URPGMMC_AttackFinal::URPGMMC_AttackFinal()
{
	AttackBaseDef  = FGameplayEffectAttributeCaptureDefinition(URPGPrimaryAttributeSet::GetAttackBaseAttribute(),  EGameplayEffectAttributeCaptureSource::Target, false);
	AttackBonusDef = FGameplayEffectAttributeCaptureDefinition(URPGPrimaryAttributeSet::GetAttackBonusAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
	AttackMulDef   = FGameplayEffectAttributeCaptureDefinition(URPGPrimaryAttributeSet::GetAttackMulAttribute(),   EGameplayEffectAttributeCaptureSource::Target, false);

	RelevantAttributesToCapture.Add(AttackBaseDef);
	RelevantAttributesToCapture.Add(AttackBonusDef);
	RelevantAttributesToCapture.Add(AttackMulDef);
}

float URPGMMC_AttackFinal::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = SourceTags;
	EvalParams.TargetTags = TargetTags;

	float Base  = 0.f;
	float Bonus = 0.f;
	float Mul   = 0.f;

	GetCapturedAttributeMagnitude(AttackBaseDef,  Spec, EvalParams, Base);
	GetCapturedAttributeMagnitude(AttackBonusDef, Spec, EvalParams, Bonus);
	GetCapturedAttributeMagnitude(AttackMulDef,   Spec, EvalParams, Mul);

	return Base * (1.f + Mul) + Bonus;
}
