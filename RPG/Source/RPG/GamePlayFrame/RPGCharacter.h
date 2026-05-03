// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "../AbilitySystem/RPGAbilitySet.h"
#include "RPGCharacter.generated.h"

class URPGAbilitySystemComponent;
class UAbilitySystemComponent;
class URPGInputConfig;

/**
 * ARPGCharacter
 *
 * 角色基类，负责：
 * 1. 实现 IAbilitySystemInterface，将 ASC 查询转发给 PlayerState
 * 2. 绑定 EnhancedInput Action，将按键映射为 InputTag
 * 3. 将 InputTag 通过 ASC::ProcessAbilityInput 传递给 GAS
 *
 * 输入流程图：
 * [按键] → [EnhancedInput Action] → [RPGCharacter::OnInputTagPressed/Released]
 *        → [ASC::ProcessAbilityInput(InputTag, bPressed)] → [缓存输入状态]
 *        → [PlayerController::PostProcessInput 中调用 ASC::ProcessAbilityInput()]
 *        → [遍历缓存，尝试激活匹配 InputTag 的能力]
 */
UCLASS()
class RPG_API ARPGCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ARPGCharacter();

	// ~IAbilitySystemInterface
	// 将查询转发给 PlayerState 中的 ASC
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~IAbilitySystemInterface

	/** 类型安全的 ASC 获取方式 */
	URPGAbilitySystemComponent* GetRPGAbilitySystemComponent() const;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** 在 Possessed 时初始化 ASC 的 ActorInfo（绑定 Pawn 作为 Avatar） */
	virtual void PossessedBy(AController* NewController) override;

	/** 在网络客户端，PlayerState 复制后初始化 ASC */
	virtual void OnRep_PlayerState() override;

	// ========================================================================
	// 内部状态
	// ========================================================================

	/** 缓存的 ASC 指针，避免每次查询 PlayerState */
	UPROPERTY()
	mutable URPGAbilitySystemComponent* CachedASC;

	/** 授予测试能力（GA_TestA）并配置 InputTag */
	void GiveTestAbility();

	// ========================================================================
	// AbilitySet 支持 - 数据驱动的能力授予
	// ========================================================================

	/** 
	 * 默认授予的 AbilitySet 列表
	 * 在 BeginPlay 中自动授予（仅在 Server/单机有效）
	 * 
	 * 使用方式：
	 *   1. 创建 URPGAbilitySet 数据资产（Content Browser → Data Asset → URPGAbilitySet）
	 *   2. 在资产中配置要授予的 Abilities、GameplayEffects、AttributeSets
	 *   3. 将此资产添加到此数组
	 *   4. 无需修改 C++ 代码，Play 时自动授予
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Ability|Ability Sets")
	TArray<TObjectPtr<URPGAbilitySet>> DefaultAbilitySets;

	/** 记录 DefaultAbilitySets 授予后的 Handles，用于 EndPlay 时撤销 */
	TArray<FRPGAbilitySet_GrantedHandles> DefaultAbilitySetHandles;

	/** 授予所有 DefaultAbilitySets */
	void GiveDefaultAbilitySets();

	/** 撤销所有 DefaultAbilitySets */
	void RemoveDefaultAbilitySets();
};
