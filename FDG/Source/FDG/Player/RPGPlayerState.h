// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "RPGPlayerState.generated.h"

class URPGAbilitySystemComponent;
class URPGHealthSet;
class URPGCombatSet;
class URPGEnergySet;

/**
 * ARPGPlayerState
 *
 *	玩家状态类，持有 AbilitySystemComponent 和战斗属性集。
 *	ASC 由 PlayerState 持有，以确保跨 Pawn 死亡/重生时能力不丢失。
 *	AttributeSet 作为默认子对象创建，确保在 ASC 初始化前即可使用。
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

	// ─── AttributeSet 访问器 ───

	URPGHealthSet* GetHealthSet() const { return HealthSet; }
	URPGCombatSet* GetCombatSet() const { return CombatSet; }
	URPGEnergySet* GetEnergySet() const { return EnergySet; }

	// ─── 便捷属性访问 ───

	float GetHealth() const;
	float GetMaxHealth() const;
	float GetEnergy() const;
	float GetMaxEnergy() const;
	float GetAttack() const;
	float GetDefense() const;

protected:

	// Health 降至 0 时的回调
	void OnHealthDepleted();

private:

	// The ability system component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG|AbilitySystem", meta = (AllowPrivateAccess = "true"))
	URPGAbilitySystemComponent* AbilitySystemComponent;

	// ─── AttributeSet（作为默认子对象创建） ───

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = "true"))
	URPGHealthSet* HealthSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = "true"))
	URPGCombatSet* CombatSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG|Attributes", meta = (AllowPrivateAccess = "true"))
	URPGEnergySet* EnergySet;
};
