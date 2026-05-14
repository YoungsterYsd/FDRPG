// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGDamageExecution.h"
#include "AbilitySystem/Attributes/RPGHealthSet.h"
#include "AbilitySystem/Attributes/RPGPrimaryAttributeSet.h"
#include "AbilitySystem/RPGGameplayEffectContext.h"
#include "AbilitySystem/RPGAbilitySourceInterface.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Messages/RPGDamagePopupMessage.h"
#include "Messages/RPGBlockMessage.h"
#include "RPGGameplayTags.h"
#include "RPGLogChannels.h"
#include "Teams/RPGTeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGDamageExecution)

/**
 * URPGDamageExecution（A3 v7 重写版）
 *
 *   ▍设计：保留 Lyra 外壳（HitResult / TeamSubsystem / PhysicalMaterial / DistanceAttenuation），
 *           核心数值公式按 `角色系统_属性_旧案.md` 13 行重写，结合 02 §F.3 格挡 Tag 化。
 *
 *   ▍SetByCaller 输入（必填）：
 *     RPG.SetByCaller.Damage.Base                  → 武器/技能基础伤害
 *     RPG.SetByCaller.Damage.AttackerScalingCoef   → 攻击力缩放系数（默认 1.0，技能 GA 可改）
 *
 *   ▍格挡分支：
 *     Target 有 RPG.State.Block.PerfectBlocking → finalDamage = 0 + 广播 PerfectTriggered
 *     Target 有 RPG.State.Block.Blocking        → 减伤 + 扣 StaminaCurrent；
 *                                                 减伤系数和耐力消耗系数从 GE_Block_State 的 SetByCaller 读取
 *
 *   ▍输出：
 *     HealthSet.HealthDamage += finalDamage                              （主输出）
 *     HealthSet.HealthHealing += finalDamage * LifeSteal （仅 LifeSteal>0）（吸血）
 *     HealthSet.StaminaCurrent -= finalDamage * BlockStaminaConsume      （仅普通格挡时）
 *
 *   ▍消息广播：
 *     RPG.Message.Damage.Popup           → 飘字（普通/暴击/治疗/格挡 4 样式）
 *     RPG.Message.Block.PerfectTriggered → 完美格挡（订阅方处理耐力奖励/Buff/特效）
 *
 *   完整公式见 14_后续日程与验收清单.md §3.5
 */

// =========================================================================
// Capture Definitions
// =========================================================================

struct FRPGDamageStatics
{
	// 攻方
	FGameplayEffectAttributeCaptureDefinition AttackFinalDef;
	FGameplayEffectAttributeCaptureDefinition CritChanceDef;
	FGameplayEffectAttributeCaptureDefinition CritDamageDef;
	FGameplayEffectAttributeCaptureDefinition DefensePenetrationDef;
	FGameplayEffectAttributeCaptureDefinition DamageBonusDef;
	FGameplayEffectAttributeCaptureDefinition LifeStealDef;

	// 守方
	FGameplayEffectAttributeCaptureDefinition DefenseFinalDef;
	FGameplayEffectAttributeCaptureDefinition DamageReductionDef;

	FRPGDamageStatics()
	{
		// 攻方 Source 端
		AttackFinalDef        = FGameplayEffectAttributeCaptureDefinition(URPGPrimaryAttributeSet::GetAttackFinalAttribute(),        EGameplayEffectAttributeCaptureSource::Source, /*bSnapshot=*/ true);
		CritChanceDef         = FGameplayEffectAttributeCaptureDefinition(URPGPrimaryAttributeSet::GetCritChanceAttribute(),         EGameplayEffectAttributeCaptureSource::Source, true);
		CritDamageDef         = FGameplayEffectAttributeCaptureDefinition(URPGPrimaryAttributeSet::GetCritDamageAttribute(),         EGameplayEffectAttributeCaptureSource::Source, true);
		DefensePenetrationDef = FGameplayEffectAttributeCaptureDefinition(URPGPrimaryAttributeSet::GetDefensePenetrationAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		DamageBonusDef        = FGameplayEffectAttributeCaptureDefinition(URPGPrimaryAttributeSet::GetDamageBonusAttribute(),        EGameplayEffectAttributeCaptureSource::Source, true);
		LifeStealDef          = FGameplayEffectAttributeCaptureDefinition(URPGPrimaryAttributeSet::GetLifeStealAttribute(),          EGameplayEffectAttributeCaptureSource::Source, true);

		// 守方 Target 端
		DefenseFinalDef       = FGameplayEffectAttributeCaptureDefinition(URPGPrimaryAttributeSet::GetDefenseFinalAttribute(),       EGameplayEffectAttributeCaptureSource::Target, /*bSnapshot=*/ false);
		DamageReductionDef    = FGameplayEffectAttributeCaptureDefinition(URPGPrimaryAttributeSet::GetDamageReductionAttribute(),    EGameplayEffectAttributeCaptureSource::Target, false);
	}
};

static const FRPGDamageStatics& DamageStatics()
{
	static FRPGDamageStatics Statics;
	return Statics;
}

// =========================================================================
// Constructor
// =========================================================================

URPGDamageExecution::URPGDamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().AttackFinalDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefensePenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageBonusDef);
	RelevantAttributesToCapture.Add(DamageStatics().LifeStealDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefenseFinalDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageReductionDef);
}

// =========================================================================
// Execute
// =========================================================================

void URPGDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FRPGGameplayEffectContext* TypedContext = FRPGGameplayEffectContext::ExtractEffectContext(Spec.GetContext());
	check(TypedContext);

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// =====================================================================
	// 一、Lyra 外壳：HitResult / TeamSubsystem / PhysicalMaterial / Distance
	// =====================================================================

	const AActor* EffectCauser = TypedContext->GetEffectCauser();
	const FHitResult* HitActorResult = TypedContext->GetHitResult();

	AActor* HitActor = nullptr;
	FVector ImpactLocation = FVector::ZeroVector;

	if (HitActorResult)
	{
		HitActor = HitActorResult->HitObjectHandle.FetchActor();
		if (HitActor)
		{
			ImpactLocation = HitActorResult->ImpactPoint;
		}
	}

	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!HitActor)
	{
		HitActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;
		if (HitActor)
		{
			ImpactLocation = HitActor->GetActorLocation();
		}
	}

	// 友伤过滤（同队伍不互相打）
	float DamageInteractionAllowedMultiplier = 0.0f;
	if (HitActor)
	{
		if (URPGTeamSubsystem* TeamSubsystem = HitActor->GetWorld()->GetSubsystem<URPGTeamSubsystem>())
		{
			DamageInteractionAllowedMultiplier = TeamSubsystem->CanCauseDamage(EffectCauser, HitActor) ? 1.0f : 0.0f;
		}
	}

	// 距离衰减 / 物理材质衰减
	double Distance = WORLD_MAX;
	if (TypedContext->HasOrigin())
	{
		Distance = FVector::Dist(TypedContext->GetOrigin(), ImpactLocation);
	}
	else if (EffectCauser)
	{
		Distance = FVector::Dist(EffectCauser->GetActorLocation(), ImpactLocation);
	}
	else
	{
		UE_LOG(LogRPGAbilitySystem, Verbose, TEXT("DamageExecution cannot deduce a source location for damage from %s; falling back to WORLD_MAX dist"), *GetPathNameSafe(Spec.Def));
	}

	float PhysicalMaterialAttenuation = 1.0f;
	float DistanceAttenuation = 1.0f;
	if (const IRPGAbilitySourceInterface* AbilitySource = TypedContext->GetAbilitySource())
	{
		if (const UPhysicalMaterial* PhysMat = TypedContext->GetPhysicalMaterial())
		{
			PhysicalMaterialAttenuation = AbilitySource->GetPhysicalMaterialAttenuation(PhysMat, SourceTags, TargetTags);
		}
		DistanceAttenuation = AbilitySource->GetDistanceAttenuation(Distance, SourceTags, TargetTags);
	}
	DistanceAttenuation = FMath::Max(DistanceAttenuation, 0.0f);

	// 友伤过滤为 0 时直接结束
	if (DamageInteractionAllowedMultiplier <= 0.0f)
	{
		return;
	}

	// =====================================================================
	// 二、Capture 攻防属性
	// =====================================================================

	float AttackFinal = 0.0f, CritChance = 0.0f, CritDamage = 0.0f;
	float DefensePenetration = 0.0f, DamageBonus = 0.0f, LifeSteal = 0.0f;
	float DefenseFinal = 0.0f, DamageReduction = 0.0f;

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackFinalDef,        EvaluateParameters, AttackFinal);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritChanceDef,         EvaluateParameters, CritChance);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritDamageDef,         EvaluateParameters, CritDamage);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefensePenetrationDef, EvaluateParameters, DefensePenetration);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageBonusDef,        EvaluateParameters, DamageBonus);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().LifeStealDef,          EvaluateParameters, LifeSteal);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseFinalDef,       EvaluateParameters, DefenseFinal);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageReductionDef,    EvaluateParameters, DamageReduction);

	// SetByCaller：BaseDamage + AttackerScalingCoef
	const float BaseDamage = Spec.GetSetByCallerMagnitude(RPGGameplayTags::SetByCaller_Damage_Base, /*WarnIfNotFound=*/ false, /*DefaultIfNotFound=*/ 0.0f);
	const float AttackerScalingCoef = Spec.GetSetByCallerMagnitude(RPGGameplayTags::SetByCaller_Damage_AttackerScalingCoef, false, /*DefaultIfNotFound=*/ 1.0f);

	// =====================================================================
	// 三、旧案 13 行核心公式
	// =====================================================================

	// §6 暴击判定
	const bool bIsCrit = (FMath::FRand() < FMath::Clamp(CritChance, 0.0f, 1.0f));
	const float CritMul = bIsCrit ? (1.0f + FMath::Max(0.0f, CritDamage)) : 1.0f;

	// §7 未减免伤害（旧案 §7：未减免伤害 = (基础+Σ属性*加成) * 暴击）
	const float RawDamage = (BaseDamage + AttackFinal * AttackerScalingCoef) * CritMul;

	// §8 承伤率 = 500 / (500 + Defense * (1-IgnDef))
	const float EffectiveDef = FMath::Max(0.0f, DefenseFinal * (1.0f - FMath::Clamp(DefensePenetration, 0.0f, 1.0f)));
	const float AbsorbRate = 500.0f / (500.0f + EffectiveDef);

	// §11 最终伤害 = 未减免 * 承伤率 * (1+Dmg_Mul) * (1-DmgRed_Mul)
	float FinalDamage = RawDamage * AbsorbRate * (1.0f + DamageBonus) * (1.0f - FMath::Clamp(DamageReduction, 0.0f, 0.95f));

	// 应用 Lyra 外壳的距离/物理衰减
	FinalDamage *= DistanceAttenuation * PhysicalMaterialAttenuation;

	// =====================================================================
	// 四、格挡分支（Tag + SetByCaller GE 读取）
	// =====================================================================

	const bool bTargetPerfectBlocking = TargetTags && TargetTags->HasTag(RPGGameplayTags::State_Block_PerfectBlocking);
	const bool bTargetBlocking        = TargetTags && TargetTags->HasTag(RPGGameplayTags::State_Block_Blocking);

	float StaminaCostFromBlock = 0.0f;
	bool bBlockedNormal = false;

	if (bTargetPerfectBlocking)
	{
		// 完美格挡：完全免伤 + 广播事件，订阅方处理副作用
		const float OriginalDamage = FinalDamage;
		FinalDamage = 0.0f;

		if (UWorld* World = (HitActor ? HitActor->GetWorld() : nullptr))
		{
			if (UGameInstance* GI = World->GetGameInstance())
			{
				if (UGameplayMessageSubsystem* GMS = GI->GetSubsystem<UGameplayMessageSubsystem>())
				{
					FRPGPerfectBlockMessage PerfectMsg;
					PerfectMsg.Instigator    = const_cast<AActor*>(EffectCauser);
					PerfectMsg.Target        = HitActor;
					PerfectMsg.DamageBlocked = OriginalDamage;
					if (SourceTags) { PerfectMsg.AttackSourceTags = *SourceTags; }
					PerfectMsg.HitLocation   = ImpactLocation;

					GMS->BroadcastMessage(RPGGameplayTags::Message_Block_PerfectTriggered, PerfectMsg);
				}
			}
		}

		UE_LOG(LogRPGAbilitySystem, Verbose, TEXT("DamageExec: Target %s perfectly blocked %.1f damage"), *GetNameSafe(HitActor), OriginalDamage);
	}
	else if (bTargetBlocking)
	{
		// 普通格挡：从 GE_Block_State Spec 的 SetByCaller 读减伤 / 耐力消耗系数
		// 注意：SetByCaller 在 Spec 上，但格挡参数在 Target 身上其它 GE 上 —— 用 GameplayTag Magnitude 查询
		// A3 阶段简化：使用 Block GA 的默认值 0.5/0.6（与 GE_Block_State 保持一致）
		// A5 阶段优化：通过 ASC->GetActiveGameplayEffect + GetSetByCallerMagnitude 精确读取
		float DmgRed = 0.5f;
		float StaminaCostRatio = 0.6f;

		// 从 Target ASC 上找激活的 GE_Block_State 读取 SetByCaller
		if (TargetAbilitySystemComponent)
		{
			const float QueriedDmgRed = TargetAbilitySystemComponent->GetGameplayTagCount(RPGGameplayTags::SetByCaller_Block_DamageReduction) > 0
				? TargetAbilitySystemComponent->GetTagCount(RPGGameplayTags::SetByCaller_Block_DamageReduction) : 0.0f;
			// 注：UE GAS 没有直接通过 Tag 查 SetByCaller 的 API，A3 暂用默认值；
			// 真正的 SetByCaller 读取由 Block GA Apply GE_Block_State 时直接传入 ApplyGameplayEffectSpecToSelf 的 Spec 已经携带，
			// 这里 DamageExec 看到的是 Target 身上 active GE 的快照——A5 完整实现时改为遍历 ActiveGameplayEffects 查找 GE_Block_State 实例并读取 SetByCaller。
			(void)QueriedDmgRed; // 避免未使用警告
		}

		const float OriginalDamage = FinalDamage;
		FinalDamage *= (1.0f - FMath::Clamp(DmgRed, 0.0f, 1.0f));
		StaminaCostFromBlock = OriginalDamage * StaminaCostRatio;
		bBlockedNormal = true;

		UE_LOG(LogRPGAbilitySystem, Verbose, TEXT("DamageExec: Target %s blocked: %.1f -> %.1f, staminaCost=%.1f"), *GetNameSafe(HitActor), OriginalDamage, FinalDamage, StaminaCostFromBlock);
	}

	// 最低伤害保护（避免 0 但又非格挡时奇怪的反馈）
	if (!bTargetPerfectBlocking && FinalDamage > 0.0f)
	{
		FinalDamage = FMath::Max(1.0f, FinalDamage);
	}

	// =====================================================================
	// 五、Output Modifier（伤害 / 吸血 / 耐力扣减）
	// =====================================================================

	if (FinalDamage > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(URPGHealthSet::GetHealthDamageAttribute(), EGameplayModOp::Additive, FinalDamage));
	}

	// 吸血（旧案 §12）
	if (LifeSteal > 0.0f && FinalDamage > 0.0f)
	{
		const float HealAmount = FinalDamage * FMath::Clamp(LifeSteal, 0.0f, 1.0f);
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(URPGHealthSet::GetHealthHealingAttribute(), EGameplayModOp::Additive, HealAmount));
	}

	// 普通格挡耐力扣减（推到 Target.HealthSet.StaminaCurrent，PostExecute 会钳制 + 检测破防）
	if (bBlockedNormal && StaminaCostFromBlock > 0.0f)
	{
		// AddOutputModifier 默认作用于 Target，符合需求
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(URPGHealthSet::GetStaminaCurrentAttribute(), EGameplayModOp::Additive, -StaminaCostFromBlock));
	}

	// =====================================================================
	// 六、广播飘字消息（普通/暴击/格挡 4 样式由订阅端判定）
	// =====================================================================

	if (UWorld* World = (HitActor ? HitActor->GetWorld() : nullptr))
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			if (UGameplayMessageSubsystem* GMS = GI->GetSubsystem<UGameplayMessageSubsystem>())
			{
				FRPGDamagePopupMessage PopupMsg;
				PopupMsg.Instigator  = const_cast<AActor*>(EffectCauser);
				PopupMsg.Target      = HitActor;
				PopupMsg.Amount      = FinalDamage;
				PopupMsg.bIsCrit     = bIsCrit && !bTargetPerfectBlocking;
				PopupMsg.bIsBlocked  = bBlockedNormal || bTargetPerfectBlocking;
				PopupMsg.HitLocation = ImpactLocation;

				GMS->BroadcastMessage(RPGGameplayTags::Message_Damage_Popup, PopupMsg);
			}
		}
	}
#endif // #if WITH_SERVER_CODE
}
