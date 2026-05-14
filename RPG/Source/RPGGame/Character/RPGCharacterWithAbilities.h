// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Character/RPGCharacter.h"

#include "RPGCharacterWithAbilities.generated.h"

#define UE_API RPGGAME_API

class UAbilitySystemComponent;
class URPGAbilitySystemComponent;
class URPGPawnData;
class UObject;

// ARPGCharacter typically gets the ability system component from the possessing player state.
// This represents a character with a self-contained ability system component.
//
// A3-1 起这个类用于玩家本体（不依赖 PlayerState 的 ASC，便于编辑器直接放置 + 单机测试）。
// A3 修订（2026-05-15）：
//   - 通过 PawnExtComp::InitializeAbilitySystem 走标准 ASC 绑定（替代原裸 InitAbilityActorInfo）
//   - PostInitializeComponents / NotifyControllerChanged 两个时机尝试 Give PawnData.AbilitySets
//   - 仅 HasAuthority 端 Give（GAS 自动复制到客户端）
UCLASS(MinimalAPI, Blueprintable)
class ARPGCharacterWithAbilities : public ARPGCharacter
{
	GENERATED_BODY()

public:
	UE_API ARPGCharacterWithAbilities(const FObjectInitializer& ObjectInitializer);

	UE_API virtual void PostInitializeComponents() override;
	UE_API virtual void NotifyControllerChanged() override;

	UE_API virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:

	/** 从 PawnData 取出 AbilitySets 并 Give 到自己的 ASC（Idempotent，bAbilitySetGranted 守护）。 */
	UE_API void GrantAbilitySetsFromPawnData(const URPGPawnData* InPawnData);

private:

	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "RPG|PlayerState")
	TObjectPtr<URPGAbilitySystemComponent> AbilitySystemComponent;

	// Health attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class URPGHealthSet> HealthSet;

	// === A3 v7：RPG primary attribute set ===
	UPROPERTY()
	TObjectPtr<const class URPGPrimaryAttributeSet> PrimaryAttributeSet;

	/** 防止重复授予（PostInitializeComponents + NotifyControllerChanged 都会尝试一次）。 */
	bool bAbilitySetGranted = false;
};

#undef UE_API
