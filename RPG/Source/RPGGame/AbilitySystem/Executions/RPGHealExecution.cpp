// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGHealExecution.h"
#include "AbilitySystem/Attributes/RPGHealthSet.h"
#include "RPGGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGHealExecution)


URPGHealExecution::URPGHealExecution()
{
	// A3 v7：不再 Capture CombatSet.BaseHeal，治疗量改由 GE Spec SetByCaller 传入（RPG.SetByCaller.Heal）
}

void URPGHealExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// A3 v7：治疗量由调用方通过 SetByCaller 注入 —— 删除 URPGCombatSet 后的新管道
	const float BaseHeal = Spec.GetSetByCallerMagnitude(RPGGameplayTags::SetByCaller_Heal, /*WarnIfNotFound=*/ false, /*DefaultIfNotFound=*/ 0.0f);

	const float HealingDone = FMath::Max(0.0f, BaseHeal);

	if (HealingDone > 0.0f)
	{
		// Apply a healing modifier, this gets turned into + HealthFinal on the target via HealthSet::PostGameplayEffectExecute
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(URPGHealthSet::GetHealthHealingAttribute(), EGameplayModOp::Additive, HealingDone));
	}
#endif // #if WITH_SERVER_CODE
}
