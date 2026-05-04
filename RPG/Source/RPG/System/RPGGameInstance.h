// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RPGGameInstance.generated.h"

// 前置声明
class URPGCharacterConfig;
class ARPGCharacter;

/**
 * URPGGameInstance
 *
 * 游戏实例，管理全局角色配置。
 * 整个游戏生命周期只存在一个实例，适合存储全局配置。
 *
 * 功能：
 * 1. 持有当前激活的角色配置（ActiveCharacterConfig）
 * 2. 支持运行时切换配置
 * 3. 记录所有已授予的 AbilitySet Handle，用于切换时清理
 */
UCLASS()
class RPG_API URPGGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	URPGGameInstance();

public:
	/** 当前激活的角色配置（整个项目只有一份） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	TObjectPtr<URPGCharacterConfig> ActiveCharacterConfig;

	/** 运行时切换到新的配置 */
	UFUNCTION(BlueprintCallable, Category = "Config")
	void SwitchCharacterConfig(URPGCharacterConfig* NewConfig);

	/** 获取当前配置的单例访问方式 */
	UFUNCTION(BlueprintPure, Category = "Config", meta = (WorldContext = "WorldContextObject"))
	static URPGGameInstance* GetRPGGameInstance(UObject* WorldContextObject);

	/** 初始化指定 Character 的技能和属性（根据 CharacterID） */
	UFUNCTION(BlueprintCallable, Category = "Config")
	void InitCharacter(ARPGCharacter* Character);

protected:
	/** 记录所有已授予的 AbilitySet Handle，用于切换时清理 */
	// Key: Character 指针, Value: 授予的 Handles 列表
	TMap<TWeakObjectPtr<AActor>, TArray<struct FRPGAbilitySet_GrantedHandles>> ActorGrantedHandles;
};
