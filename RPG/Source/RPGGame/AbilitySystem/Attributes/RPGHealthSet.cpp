// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGHealthSet.h"
#include "AbilitySystem/Attributes/RPGAttributeSet.h"
#include "RPGGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameplayEffectExtension.h"
#include "Messages/RPGVerbMessage.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGHealthSet)

// === Lyra 沿用 Damage 流程 Tag（与原版一致）===
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_Damage,             "Gameplay.Damage");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageImmunity,     "Gameplay.DamageImmunity");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageSelfDestruct, "Gameplay.Damage.SelfDestruct");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_FellOutOfWorld,     "Gameplay.Damage.FellOutOfWorld");
UE_DEFINE_GAMEPLAY_TAG(TAG_RPG_Damage_Message,          "RPG.Damage.Message");

URPGHealthSet::URPGHealthSet()
{
	// v7 锁定：C++ 不硬编码任何初值。
	// 由 GE_HealthSet_Init (Instant) 赋 StaminaMax=100 / StaminaCurrent=100，
	// 由 GE_Health_Derive_Max (Infinite + AttributeBased) 派生写入 HealthMax，
	// 由 GE_Health_Init_Full (Instant + AttributeBased) 在 HealthMax 派生完成后写入 HealthFinal=HealthMax。
	// 详见 14_后续日程与验收清单.md §3.3.4
}

void URPGHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URPGHealthSet, HealthFinal,    COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGHealthSet, HealthMax,      COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGHealthSet, StaminaCurrent, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGHealthSet, StaminaMax,     COND_None, REPNOTIFY_Always);
}

// =========================================================================
// OnRep
// =========================================================================

void URPGHealthSet::OnRep_HealthFinal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGHealthSet, HealthFinal, OldValue);

	const float CurrentHealth = GetHealthFinal();
	const float EstimatedMagnitude = CurrentHealth - OldValue.GetCurrentValue();

	OnHealthChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);

	if (!bOutOfHealth && CurrentHealth <= 0.0f)
	{
		OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);
	}

	bOutOfHealth = (CurrentHealth <= 0.0f);
}

void URPGHealthSet::OnRep_HealthMax(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGHealthSet, HealthMax, OldValue);

	OnMaxHealthChanged.Broadcast(nullptr, nullptr, nullptr, GetHealthMax() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetHealthMax());
}

void URPGHealthSet::OnRep_StaminaCurrent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGHealthSet, StaminaCurrent, OldValue);

	const float CurrentStamina = GetStaminaCurrent();
	if (!bBlockBroken && CurrentStamina <= 0.0f)
	{
		OnBlockBroken.Broadcast(nullptr, nullptr, nullptr, CurrentStamina - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), CurrentStamina);
	}
	bBlockBroken = (CurrentStamina <= 0.0f);
}

void URPGHealthSet::OnRep_StaminaMax(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGHealthSet, StaminaMax, OldValue);
}

// =========================================================================
// PreGameplayEffectExecute（伤害免疫 / GodMode cheat）
// =========================================================================

bool URPGHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	// 处理流入伤害（HealthDamage Meta）
	if (Data.EvaluatedData.Attribute == GetHealthDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);

			if (Data.Target.HasMatchingGameplayTag(TAG_Gameplay_DamageImmunity) && !bIsDamageFromSelfDestruct)
			{
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}

#if !UE_BUILD_SHIPPING
			if (Data.Target.HasMatchingGameplayTag(RPGGameplayTags::Cheat_GodMode) && !bIsDamageFromSelfDestruct)
			{
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}
#endif
		}
	}

	// 缓存变化前的值
	HealthBeforeAttributeChange    = GetHealthFinal();
	HealthMaxBeforeAttributeChange = GetHealthMax();
	StaminaBeforeAttributeChange   = GetStaminaCurrent();

	return true;
}

// =========================================================================
// PostGameplayEffectExecute（Meta 转换 + 钳制 + 广播）
// =========================================================================

void URPGHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);
	float MinimumHealth = 0.0f;

#if !UE_BUILD_SHIPPING
	if (!bIsDamageFromSelfDestruct &&
		(Data.Target.HasMatchingGameplayTag(RPGGameplayTags::Cheat_GodMode) ||
		 Data.Target.HasMatchingGameplayTag(RPGGameplayTags::Cheat_UnlimitedHealth)))
	{
		MinimumHealth = 1.0f;
	}
#endif

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetHealthDamageAttribute())
	{
		// 兼容广播：发布 RPG.Damage.Message verb（Lyra 沿用），仅当 GameInstance 上的 GMS 可用时
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			FRPGVerbMessage Message;
			Message.Verb = TAG_RPG_Damage_Message;
			Message.Instigator = Causer;
			Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
			Message.Target = GetOwningActor();
			Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
			Message.Magnitude = Data.EvaluatedData.Magnitude;

			UWorld* World = GetWorld();
			UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
			if (GI)
			{
				if (UGameplayMessageSubsystem* GMS = GI->GetSubsystem<UGameplayMessageSubsystem>())
				{
					GMS->BroadcastMessage(Message.Verb, Message);
				}
			}
		}

		// HealthDamage → -HealthFinal，钳制
		SetHealthFinal(FMath::Clamp(GetHealthFinal() - GetHealthDamage(), MinimumHealth, GetHealthMax()));
		SetHealthDamage(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthHealingAttribute())
	{
		// HealthHealing → +HealthFinal，钳制
		SetHealthFinal(FMath::Clamp(GetHealthFinal() + GetHealthHealing(), MinimumHealth, GetHealthMax()));
		SetHealthHealing(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthFinalAttribute())
	{
		// 直接修改 HealthFinal（如复活/治疗 GE Override），钳制
		SetHealthFinal(FMath::Clamp(GetHealthFinal(), MinimumHealth, GetHealthMax()));
	}
	else if (Data.EvaluatedData.Attribute == GetHealthMaxAttribute())
	{
		// HealthMax 变化：按比例同步 HealthFinal，避免"加 100 满血但 HealthFinal 还停留在 50/50"的尴尬
		const float OldMax = HealthMaxBeforeAttributeChange;
		const float NewMax = GetHealthMax();
		if (OldMax > 0.0f && !FMath::IsNearlyEqual(OldMax, NewMax))
		{
			const float Ratio = HealthBeforeAttributeChange / OldMax;
			SetHealthFinal(FMath::Clamp(Ratio * NewMax, MinimumHealth, NewMax));
		}
		OnMaxHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, OldMax, NewMax);
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaCurrentAttribute())
	{
		// 钳制
		SetStaminaCurrent(FMath::Clamp(GetStaminaCurrent(), 0.0f, GetStaminaMax()));
	}

	// === HealthFinal 变化广播 ===
	if (GetHealthFinal() != HealthBeforeAttributeChange)
	{
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealthFinal());
	}

	// === HealthFinal 到 0 → OnOutOfHealth + 广播 RPG.Message.Health.OutOfHealth ===
	if ((GetHealthFinal() <= 0.0f) && !bOutOfHealth)
	{
		OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealthFinal());

		// v7 新增：通过 GameplayMessage 广播 OutOfHealth（订阅方在 A6 死亡管线接入）
		UWorld* World = GetWorld();
		UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
		if (GI)
		{
			if (UGameplayMessageSubsystem* GMS = GI->GetSubsystem<UGameplayMessageSubsystem>())
			{
				FRPGVerbMessage OutOfHealthMsg;
				OutOfHealthMsg.Verb = RPGGameplayTags::Message_Health_OutOfHealth;
				OutOfHealthMsg.Instigator = Causer;
				OutOfHealthMsg.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
				OutOfHealthMsg.Target = GetOwningActor();
				OutOfHealthMsg.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
				OutOfHealthMsg.Magnitude = Data.EvaluatedData.Magnitude;

				GMS->BroadcastMessage(OutOfHealthMsg.Verb, OutOfHealthMsg);
			}
		}
	}
	bOutOfHealth = (GetHealthFinal() <= 0.0f);

	// === StaminaCurrent 变化广播 ===
	if (GetStaminaCurrent() != StaminaBeforeAttributeChange)
	{
		// StaminaCurrent → 0 触发破防广播（一次性）
		if ((GetStaminaCurrent() <= 0.0f) && !bBlockBroken)
		{
			OnBlockBroken.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, StaminaBeforeAttributeChange, GetStaminaCurrent());

			UWorld* World = GetWorld();
			UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
			if (GI)
			{
				if (UGameplayMessageSubsystem* GMS = GI->GetSubsystem<UGameplayMessageSubsystem>())
				{
					FRPGVerbMessage BlockBrokenMsg;
					BlockBrokenMsg.Verb = RPGGameplayTags::Message_Block_Broken;
					BlockBrokenMsg.Instigator = Causer;
					BlockBrokenMsg.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
					BlockBrokenMsg.Target = GetOwningActor();
					BlockBrokenMsg.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
					BlockBrokenMsg.Magnitude = Data.EvaluatedData.Magnitude;

					GMS->BroadcastMessage(BlockBrokenMsg.Verb, BlockBrokenMsg);
				}
			}

			// 给目标加 LooseGameplayTag State.Block.Broken，A5 破防 GA 订阅
			if (UAbilitySystemComponent* TargetASC = Data.Target.AbilityActorInfo.IsValid() ? Data.Target.AbilityActorInfo->AbilitySystemComponent.Get() : nullptr)
			{
				TargetASC->AddLooseGameplayTag(RPGGameplayTags::State_Block_Broken);
			}
		}
		bBlockBroken = (GetStaminaCurrent() <= 0.0f);
	}
}

// =========================================================================
// Pre/Post Attribute Change（钳制）
// =========================================================================

void URPGHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void URPGHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void URPGHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	// HealthMax 通过 Modifier 直接变化（非 GE Execute）时，HealthFinal 按比例同步
	if (Attribute == GetHealthMaxAttribute() && OldValue > 0.0f && !FMath::IsNearlyEqual(OldValue, NewValue))
	{
		if (URPGAbilitySystemComponent* RPGASC = GetRPGAbilitySystemComponent())
		{
			const float Ratio = GetHealthFinal() / OldValue;
			const float NewHealthFinal = FMath::Clamp(Ratio * NewValue, 0.0f, NewValue);
			RPGASC->ApplyModToAttribute(GetHealthFinalAttribute(), EGameplayModOp::Override, NewHealthFinal);
		}
	}

	// StaminaMax 同理
	if (Attribute == GetStaminaMaxAttribute() && OldValue > 0.0f && !FMath::IsNearlyEqual(OldValue, NewValue))
	{
		if (GetStaminaCurrent() > NewValue)
		{
			if (URPGAbilitySystemComponent* RPGASC = GetRPGAbilitySystemComponent())
			{
				RPGASC->ApplyModToAttribute(GetStaminaCurrentAttribute(), EGameplayModOp::Override, NewValue);
			}
		}
	}

	// 复活后清除 OutOfHealth/BlockBroken 状态标记
	if (bOutOfHealth && (GetHealthFinal() > 0.0f))
	{
		bOutOfHealth = false;
	}
	if (bBlockBroken && (GetStaminaCurrent() > 0.0f))
	{
		bBlockBroken = false;
	}
}

void URPGHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthFinalAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetHealthMax());
	}
	else if (Attribute == GetHealthMaxAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	else if (Attribute == GetStaminaCurrentAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetStaminaMax());
	}
	else if (Attribute == GetStaminaMaxAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}
