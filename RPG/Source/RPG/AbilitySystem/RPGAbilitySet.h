// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "ActiveGameplayEffectHandle.h"
#include "AttributeSet.h"
#include "RPGAbilitySet.generated.h"

// 前置声明，避免循环依赖
class URPGAbilitySystemComponent;
class URPGGameplayAbility;
class UGameplayEffect;
class URPGGameplayAbility;
class UGameplayEffect;

/**
 * FRPGAbilitySet_Ability
 * 
 * 定义一个要授予的 GameplayAbility 及其配置
 */
USTRUCT(BlueprintType)
struct FRPGAbilitySet_Ability
{
	GENERATED_BODY()

	/** 要授予的 GameplayAbility 类 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<URPGGameplayAbility> Ability = nullptr;

	/** 此能力绑定的输入标签（用于输入触发） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	FGameplayTag InputTag;
};

/**
 * FRPGAbilitySet_GameplayEffect
 * 
 * 定义一个要授予的 GameplayEffect（如被动Buff、初始属性）
 */
USTRUCT(BlueprintType)
struct FRPGAbilitySet_GameplayEffect
{
	GENERATED_BODY()

	/** 要应用的 GameplayEffect 类 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;
};

/**
 * FRPGAbilitySet_AttributeSet
 * 
 * 定义一个要授予的 AttributeSet（属性集）
 */
USTRUCT(BlueprintType)
struct FRPGAbilitySet_AttributeSet
{
	GENERATED_BODY()

	/** 要授予的 AttributeSet 类 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute Set")
	TSubclassOf<UAttributeSet> AttributeSet;
};

/**
 * FRPGAbilitySet_GrantedHandles
 * 
 * 记录一次 AbilitySet 授予后产生的所有 Handle，用于后续精确撤销
 * 
 * 设计说明：
 *   - 一个 AbilitySet 可能授予多个 Ability、GE、AttributeSet
 *   - 撤销时必须精确移除这些，而不能影响其他来源授予的能力
 *   - 因此需要在授予时记录所有 Handle，撤销时逐一移除
 */
struct FRPGAbilitySet_GrantedHandles
{
public:
	/** 记录授予的 Ability SpecHandle */
	TArray<FGameplayAbilitySpecHandle> AbilityHandles;

	/** 记录授予的 ActiveGameplayEffect Handle */
	TArray<FActiveGameplayEffectHandle> EffectHandles;

	/** 记录授予的 AttributeSet 指针 */
	TArray<TWeakObjectPtr<UAttributeSet>> AttributeSets;

	/** 撤销此 AbilitySet（移除所有授予的能力/GE/AttributeSet） */
	void TakeFromAbilitySystem(URPGAbilitySystemComponent* ASC);
};

/**
 * URPGAbilitySet
 * 
 * AbilitySet 数据资产：批量定义要授予的能力、GE、属性集
 * 
 * 使用场景：
 *   1. 角色初始能力（如移动、攻击、技能）
 *   2. 装备附加能力（如武器技能）
 *   3. 状态附加能力（如Buff、Debuff）
 * 
 * 优势：
 *   - 批量管理：一个 Set 包含多个能力，便于打包/替换
 *   - 精确撤销：撤销时只移除此 Set 授予的内容，不影响其他
 *   - 数据驱动：不需要改 C++ 代码，只需配置资产
 */
UCLASS(BlueprintType)
class URPGAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	URPGAbilitySet() = default;

public:
	/** 要授予的能力列表 */
	UPROPERTY(EditDefaultsOnly, Category = "Ability Set", meta = (TitleProperty = "Ability"))
	TArray<FRPGAbilitySet_Ability> Abilities;

	/** 要应用的 GameplayEffect 列表 */
	UPROPERTY(EditDefaultsOnly, Category = "Ability Set", meta = (TitleProperty = "GameplayEffect"))
	TArray<FRPGAbilitySet_GameplayEffect> GameplayEffects;

	/** 要授予的 AttributeSet 列表 */
	UPROPERTY(EditDefaultsOnly, Category = "Ability Set", meta = (TitleProperty = "AttributeSet"))
	TArray<FRPGAbilitySet_AttributeSet> AttributeSets;

public:
	/** 授予此 AbilitySet 到指定的 ASC */
	bool GiveToAbilitySystem(URPGAbilitySystemComponent* ASC, FRPGAbilitySet_GrantedHandles* OutGrantedHandles = nullptr) const;
};
