// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "../AbilitySystem/AbilitySet/RPGAbilitySet.h"
#include "../AbilitySystem/AttributeSystem/RPGAttributeInitTableRow.h"
#include "RPGCharacterConfig.generated.h"

/**
 * URPGCharacterConfig - 角色配置数据资产
 *
 * 集中管理角色的技能授予和属性初始化配置。
 * 在编辑器中创建为 DataAsset，然后在 GameInstance 中引用。
 *
 * 功能：
 * 1. GenericAbilitySets  - 通用技能集（所有角色共享）
 * 2. CharacterAbilitySets - 角色特定技能集（按 CharacterID 区分）
 * 3. AttributeInitTable   - 属性初始化表（CSV 数据）
 *
 * 使用流程：
 *   1. 在 Content Browser 中创建 Data Asset（父类选 URPGCharacterConfig）
 *   2. 配置通用技能集、属性初始化表、角色特定技能集
 *   3. 在 BP_RPGGameInstance 的 Class Defaults 中设置 ActiveCharacterConfig
 */
UCLASS(BlueprintType)
class RPG_API URPGCharacterConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	/** 通用技能集（所有角色/怪物共享） */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Generic")
	TArray<TObjectPtr<URPGAbilitySet>> GenericAbilitySets;

	/** 属性初始化表 */
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	TSoftObjectPtr<UDataTable> AttributeInitTable;

	/** 角色特定技能集（按 CharacterID 分组，每个 CharacterID 对应一个 AbilitySet） */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Character")
	TMap<FName, TObjectPtr<URPGAbilitySet>> CharacterAbilitySets;

public:
	/** 根据 CharacterID 获取对应的技能集（通用 + 特定） */
	UFUNCTION(BlueprintCallable, Category = "Config")
	TArray<URPGAbilitySet*> GetAbilitySetsForCharacter(FName CharacterID) const;

	/** 根据 CharacterID 从 AttributeInitTable 获取属性初始化数据 */
	const FRPGAttributeInitTableRow* GetAttributeInitData(FName CharacterID) const;

	/** Blueprint 包装函数 - 通过输出参数获取属性初始化数据 */
	UFUNCTION(BlueprintCallable, Category = "Config")
	bool GetAttributeInitDataBP(FName CharacterID, FRPGAttributeInitTableRow& OutRow) const;

	/** 获取所有已配置的 CharacterID */
	UFUNCTION(BlueprintCallable, Category = "Config")
	TArray<FName> GetAllCharacterIDs() const;
};
