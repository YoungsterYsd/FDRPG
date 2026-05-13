// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "RPGInputConfig.generated.h"

class UInputAction;
class UObject;
struct FFrame;

/**
 * FLyraInputAction
 *
 *	Struct used to map a input action to a gameplay input tag.
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
 * ULyraInputConfig
 *
 *	Non-mutable data asset that contains input configuration properties.
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

	/** A2 工具用：从 Python/BP 里向 NativeInputActions 数组追加一条映射（接收 FName 形式 Tag 以便 Python 调用） */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "RPG|Tools")
	void AddNativeMapping(const UInputAction* InputAction, FName InputTagName);

	/** A2 工具用：同上，向 AbilityInputActions 数组追加一条映射 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "RPG|Tools")
	void AddAbilityMapping(const UInputAction* InputAction, FName InputTagName);

	/** A2 工具用：清空两个数组（重新生成前调用） */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "RPG|Tools")
	void ClearAllMappings();

public:
	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and must be manually bound.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FRPGInputAction> NativeInputActions;

	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and are automatically bound to abilities with matching input tags.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FRPGInputAction> AbilityInputActions;
};
