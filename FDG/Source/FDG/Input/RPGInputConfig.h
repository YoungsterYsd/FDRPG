// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "RPGInputConfig.generated.h"

class UInputAction;

/**
 * FRPGInputAction
 *
 *	定义 InputAction -> GameplayTag 映射。
 *	bIsNative=true 的条目由 C++ 直接处理（如 Move），否则走 ASC 能力输入。
 */
USTRUCT(BlueprintType)
struct FRPGInputAction
{
	GENERATED_BODY()

public:

	// 输入动作资产（软引用，支持 GameFeature 插件延迟加载）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TSoftObjectPtr<UInputAction> InputAction;

	// 该输入动作映射的 GameplayTag
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	FGameplayTag InputTag;

	// 是否为 Native 输入（不由 ASC 处理，直接调用 C++ 回调）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	bool bIsNative = false;
};

/**
 * URPGInputConfig
 *
 *	输入配置数据资产，定义 InputAction -> GameplayTag 映射。
 *	由 HeroComponent 读取并绑定到 Native 回调或 ASC 能力输入。
 */
UCLASS(Const, BlueprintType, Meta = (DisplayName = "RPG Input Config", ShortTooltip = "Data asset mapping InputActions to GameplayTags."))
class RPGGAME_API URPGInputConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	URPGInputConfig();

	// InputAction -> Tag 映射列表
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TArray<FRPGInputAction> InputActions;

	// 根据 Tag 查找 Native 输入动作
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag) const;

	// 根据 Tag 查找 Ability 输入动作
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag) const;
};
