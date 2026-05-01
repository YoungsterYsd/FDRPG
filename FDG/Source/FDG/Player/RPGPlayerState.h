// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "RPGPlayerState.generated.h"

class URPGAbilitySystemComponent;

/**
 * ARPGPlayerState
 *
 *	玩家状态类，持有 AbilitySystemComponent。
 *	ASC 由 PlayerState 持有，以确保跨 Pawn 死亡/重生时能力不丢失。
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The player state class used by this project."))
class RPGGAME_API ARPGPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	ARPGPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	//~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface

	// Get the RPG-specific ASC
	URPGAbilitySystemComponent* GetRPGAbilitySystemComponent() const;

private:

	// The ability system component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG|AbilitySystem", meta = (AllowPrivateAccess = "true"))
	URPGAbilitySystemComponent* AbilitySystemComponent;
};
