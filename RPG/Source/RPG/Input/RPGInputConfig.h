// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "RPGInputConfig.generated.h"

class UInputAction;

/**
 * FRPGInputAction - 将InputAction与GameplayTag关联
 */
USTRUCT(BlueprintType)
struct FRPGInputAction
{
	GENERATED_BODY()

public:
	// 输入动作
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	const UInputAction* InputAction = nullptr;

	// 关联的GameplayTag（用于触发Ability或传递输入事件）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * URPGInputConfig - Input Config DataAsset
 * 用于将InputAction与GameplayTag关联，作为增强输入系统的配置中心
 */
UCLASS(BlueprintType, Const)
class RPG_API URPGInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	URPGInputConfig(const FObjectInitializer& ObjectInitializer);

	// 根据InputAction查找对应的GameplayTag
	const FGameplayTag& FindInputTagForAction(const UInputAction* InputAction) const;

	// 获取所有AbilityInputActions（只读访问）
	const TArray<FRPGInputAction>& GetAbilityInputActions() const { return AbilityInputActions; }

protected:
	// Ability输入动作数组 - 用于触发GameplayAbility
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FRPGInputAction> AbilityInputActions;
};
