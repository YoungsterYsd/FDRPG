// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Character/RPGCharacter.h"

#include "RPGCharacterWithAbilities.generated.h"

#define UE_API RPGGAME_API

class UAbilitySystemComponent;
class URPGAbilitySystemComponent;
class UObject;

// ALyraCharacter typically gets the ability system component from the possessing player state
// This represents a character with a self-contained ability system component.
UCLASS(MinimalAPI, Blueprintable)
class ARPGCharacterWithAbilities : public ARPGCharacter
{
	GENERATED_BODY()

public:
	UE_API ARPGCharacterWithAbilities(const FObjectInitializer& ObjectInitializer);

	UE_API virtual void PostInitializeComponents() override;

	UE_API virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:

	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "RPG|PlayerState")
	TObjectPtr<URPGAbilitySystemComponent> AbilitySystemComponent;
	
	// Health attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class URPGHealthSet> HealthSet;
	// Combat attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class URPGCombatSet> CombatSet;
};

#undef UE_API
