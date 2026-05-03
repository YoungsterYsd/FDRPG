// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGPlayerState.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/RPGHealthSet.h"
#include "AbilitySystem/Attributes/RPGCombatSet.h"
#include "AbilitySystem/Attributes/RPGEnergySet.h"
#include "GameplayTags/RPGGameplayTags.h"
#include "System/RPGLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGPlayerState)

ARPGPlayerState::ARPGPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<URPGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// 创建 AttributeSet 作为默认子对象
	HealthSet = CreateDefaultSubobject<URPGHealthSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<URPGCombatSet>(TEXT("CombatSet"));
	EnergySet = CreateDefaultSubobject<URPGEnergySet>(TEXT("EnergySet"));

	// 高频网络更新以确保 ASC 数据及时同步
	SetNetUpdateFrequency(100.0f);
}

void ARPGPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (AbilitySystemComponent)
	{
		// 初始化 ASC 的 Owner 和 Avatar
		// 此时 GetPawn() 可能为 null，仅设置 Owner，Avatar 在 PossessedBy 时设置
		AbilitySystemComponent->InitAbilityActorInfo(this, nullptr);

		// 注册 AttributeSet 到 ASC（AddAttributeSetSubobject 在 InitAbilityActorInfo 之后调用）
		if (HealthSet)
		{
			AbilitySystemComponent->AddAttributeSetSubobject(HealthSet);
		}
		if (CombatSet)
		{
			AbilitySystemComponent->AddAttributeSetSubobject(CombatSet);
		}
		if (EnergySet)
		{
			AbilitySystemComponent->AddAttributeSetSubobject(EnergySet);
		}

		// 绑定 HealthDepleted 事件到死亡处理
		if (HealthSet)
		{
			HealthSet->OnHealthDepleted.AddUObject(this, &ARPGPlayerState::OnHealthDepleted);
		}
	}
}

UAbilitySystemComponent* ARPGPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

URPGAbilitySystemComponent* ARPGPlayerState::GetRPGAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

float ARPGPlayerState::GetHealth() const
{
	return HealthSet ? HealthSet->GetHealth() : 0.0f;
}

float ARPGPlayerState::GetMaxHealth() const
{
	return HealthSet ? HealthSet->GetMaxHealth() : 0.0f;
}

float ARPGPlayerState::GetEnergy() const
{
	return EnergySet ? EnergySet->GetEnergy() : 0.0f;
}

float ARPGPlayerState::GetMaxEnergy() const
{
	return EnergySet ? EnergySet->GetMaxEnergy() : 0.0f;
}

float ARPGPlayerState::GetAttack() const
{
	return CombatSet ? CombatSet->GetAttack() : 0.0f;
}

float ARPGPlayerState::GetDefense() const
{
	return CombatSet ? CombatSet->GetDefense() : 0.0f;
}

void ARPGPlayerState::OnHealthDepleted()
{
	UE_LOG(LogRPG, Log, TEXT("ARPGPlayerState::OnHealthDepleted - Health depleted, activating Death ability"));

	// 通知 ASC 激活死亡 Ability
	if (AbilitySystemComponent)
	{
		// 通过 GameplayEvent 触发死亡（Death GA 监听此事件）
		FGameplayEventData EventData;
		EventData.EventTag = RPGGameplayTags::GameplayEvent_Death;
		AbilitySystemComponent->HandleGameplayEvent(RPGGameplayTags::GameplayEvent_Death, &EventData);
	}
}
