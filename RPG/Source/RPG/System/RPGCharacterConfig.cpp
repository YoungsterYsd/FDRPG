// Fill out your copyright notice in the Description page of Project Settings.

#include "RPGCharacterConfig.h"
#include "Engine/DataTable.h"
#include "../System/RPGLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGCharacterConfig)

TArray<URPGAbilitySet*> URPGCharacterConfig::GetAbilitySetsForCharacter(FName CharacterID) const
{
	// 合并通用技能集和角色特定技能集
	// 通用技能集：所有角色都会授予
	// 角色特定技能集：仅特定 CharacterID 的角色授予
	// 检查 CharacterAbilitySets 中的数据有效性
	for (const auto& Pair : CharacterAbilitySets)
	{
		if (!Pair.Value)
		{
			UE_LOG(LogRPG, Error, TEXT("GetAbilitySets: CharacterAbilitySets Key [%s] has NULL Value!"), 
				*Pair.Key.ToString());
		}
	}

	TArray<URPGAbilitySet*> Result;

	// 1. 添加通用技能集（所有角色共享）
	for (URPGAbilitySet* AbilitySet : GenericAbilitySets)
	{
		if (AbilitySet)
		{
			Result.Add(AbilitySet);
		}
	}

	// 2. 添加角色特定技能集（每个 CharacterID 对应一个 AbilitySet）
	if (const TObjectPtr<URPGAbilitySet>* FoundSet = CharacterAbilitySets.Find(CharacterID))
	{
		if (FoundSet->Get())
		{
			Result.Add(FoundSet->Get());
		}
		else
		{
			UE_LOG(LogRPG, Error, TEXT("GetAbilitySets: CharacterID [%s] found but AbilitySet is NULL!"), *CharacterID.ToString());
		}
	}

	return Result;
}

const FRPGAttributeInitTableRow* URPGCharacterConfig::GetAttributeInitData(FName CharacterID) const
{
	// 从 AttributeInitTable 中查找指定 CharacterID 的属性初始化数据
	// 返回 nullptr 表示未找到或加载失败
	if (!AttributeInitTable.IsValid())
	{
		UE_LOG(LogRPG, Error, TEXT("GetAttributeInitData: AttributeInitTable is not set!"));
		return nullptr;
	}

	UDataTable* Table = AttributeInitTable.LoadSynchronous();
	if (!Table)
	{
		UE_LOG(LogRPG, Error, TEXT("GetAttributeInitData: Failed to load AttributeInitTable!"));
		return nullptr;
	}

	const FRPGAttributeInitTableRow* Row = Table->FindRow<FRPGAttributeInitTableRow>(CharacterID, TEXT("URPGCharacterConfig::GetAttributeInitData"));
	if (!Row)
	{
		UE_LOG(LogRPG, Warning, TEXT("GetAttributeInitData: CharacterID [%s] not found in AttributeInitTable"), *CharacterID.ToString());
	}

	return Row;
}

bool URPGCharacterConfig::GetAttributeInitDataBP(FName CharacterID, FRPGAttributeInitTableRow& OutRow) const
{
	// Blueprint 包装函数 - 通过输出参数获取属性初始化数据
	const FRPGAttributeInitTableRow* Row = GetAttributeInitData(CharacterID);
	if (Row)
	{
		OutRow = *Row;
		return true;
	}

	return false;
}

TArray<FName> URPGCharacterConfig::GetAllCharacterIDs() const
{
	// 获取所有已配置的 CharacterID（来自 CharacterAbilitySets 的 Key）
	TArray<FName> Result;
	CharacterAbilitySets.GetKeys(Result);
	return Result;
}
