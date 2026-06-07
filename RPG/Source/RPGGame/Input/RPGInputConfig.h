// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "RPGInputConfig.generated.h"

class UInputAction;
class UObject;
struct FFrame;

/**
 * FRPGInputAction — 单条输入映射记录
 *
 * 承载一个 UInputAction 资产指针与对应的 FGameplayTag，
 * 是 URPGInputConfig 映射表的基本组成单元。
 * 分为两类用途：NativeInputActions（移动/视角等手动绑定）和 AbilityInputActions（技能自动绑定）。
 */
USTRUCT(BlueprintType)
struct FRPGInputAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * URPGInputConfig — 输入映射配置资产（不可变 DataAsset）
 *
 * 定义 InputAction 与 GameplayTag 的对应关系，是输入系统的配置中心。
 * - NativeInputActions：手动绑定的原生输入（移动/视角/蹲下等），由 URPGInputComponent::BindNativeAction 消费
 * - AbilityInputActions：技能输入映射，由 URPGInputComponent::BindAbilityActions 自动绑定到 GA 的 InputTag
 *
 * 本 DataAsset 在 PawnData 中引用，HeroComponent 初始化时加载。
 */
UCLASS(BlueprintType, Const)
class URPGInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	URPGInputConfig(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "RPG|Pawn")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, Category = "RPG|Pawn")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and must be manually bound.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FRPGInputAction> NativeInputActions;

	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and are automatically bound to abilities with matching input tags.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FRPGInputAction> AbilityInputActions;
};
