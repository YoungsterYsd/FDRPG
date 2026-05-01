// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RPGGameplayAbility.generated.h"

/**
 * ERPGAbilityActivationPolicy
 *
 *	能力激活策略：决定能力如何响应输入。
 */
UENUM(BlueprintType)
enum class ERPGAbilityActivationPolicy : uint8
{
	// Input triggered: Activate once when input is pressed
	OnInputTriggered,

	// While input active: Keep ability active while input is held
	WhileInputActive,

	// On spawn: Activate automatically when granted (passive abilities)
	OnSpawn,
};

/**
 * ERPGAbilityCollisionPolicy
 *
 *	能力激活时的碰撞处理策略（P2 实现）。
 */
UENUM(BlueprintType)
enum class ERPGAbilityCollisionPolicy : uint8
{
	// Do not modify collision
	NoModification,

	// Enable collision during activation
	AlwaysEnable,

	// Disable collision during activation
	AlwaysDisable,
};

/**
 * URPGGameplayAbility
 *
 *	RPG 能力基类。提供 InputTag 绑定、激活策略和工具方法。
 *	通过 OnGiveAbility 将 InputTag 写入 AbilitySpec 的 DynamicSpecSourceTags，
 *	使 ASC 的 OnAbilityInputPressed 能按 Tag 匹配并激活能力。
 */
UCLASS(Abstract, HideCategories = Input, Meta = (ShortTooltip = "The base gameplay ability class used by this project."))
class RPGGAME_API URPGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	URPGGameplayAbility();

	//~UGameplayAbility interface
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	//~End of UGameplayAbility interface

	// Returns the activation policy for this ability
	ERPGAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	// Returns the input tag associated with this ability
	FGameplayTag GetAbilityInputTag() const { return InputTag; }

protected:

	// Policy that determines how the ability is activated with respect to input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Activation")
	ERPGAbilityActivationPolicy ActivationPolicy = ERPGAbilityActivationPolicy::OnInputTriggered;

	// Input tag that triggers this ability
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Input")
	FGameplayTag InputTag;

	// Whether this ability should be canceled when the character moves
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Cancel")
	bool bCancelOnMovement = false;

	// Whether this ability should be canceled when the weapon is switched
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Cancel")
	bool bShouldCancelOnWeaponSwitch = false;

	// Whether this ability is cancelable by the player
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Cancel")
	bool bRPGCancelable = true;

	// Collision policy for this ability
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Collision")
	ERPGAbilityCollisionPolicy CollisionPolicy = ERPGAbilityCollisionPolicy::NoModification;

	// If true, this ability will survive the owner's death
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Behavior")
	bool bSurvivesDeath = false;
};
