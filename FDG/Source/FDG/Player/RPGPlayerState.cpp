// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGPlayerState.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "System/RPGLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGPlayerState)

ARPGPlayerState::ARPGPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<URPGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// 高频网络更新以确保 ASC 数据及时同步
	SetNetUpdateFrequency(100.0f);
}

void ARPGPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 此时 GetPawn() 为 null，仅设置 Owner，Avatar 在 PossessedBy 时设置
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, nullptr);
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
