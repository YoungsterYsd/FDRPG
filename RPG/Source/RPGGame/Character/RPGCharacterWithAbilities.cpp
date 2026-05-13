// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGCharacterWithAbilities.h"

#include "AbilitySystem/Attributes/RPGCombatSet.h"
#include "AbilitySystem/Attributes/RPGHealthSet.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Async/TaskGraphInterfaces.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGCharacterWithAbilities)

ARPGCharacterWithAbilities::ARPGCharacterWithAbilities(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<URPGAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	HealthSet = CreateDefaultSubobject<URPGHealthSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<URPGCombatSet>(TEXT("CombatSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);
}

void ARPGCharacterWithAbilities::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

UAbilitySystemComponent* ARPGCharacterWithAbilities::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

