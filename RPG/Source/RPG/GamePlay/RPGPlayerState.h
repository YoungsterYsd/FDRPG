// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "../AbilitySystem/AbilitySet/RPGAbilitySet.h"
#include "RPGPlayerState.generated.h"

class URPGAbilitySystemComponent;
class URPGAbilityTagRelationshipMapping;

/**
 * ARPGPlayerState
 *
 * 持有 AbilitySystemComponent，作为 GAS 网络复制的 Owner。
 * 
 * 为什么 ASC 放在 PlayerState 而不是 Character？
 * - PlayerState 在网络中存活更久，即使 Pawn 被销毁重建，ASC 和已授予的能力不会丢失
 * - 适合需要持久化能力状态的多人游戏（如等级、技能解锁等）
 * 
 * 如果纯单机游戏，ASC 也可以放在 Character 上，但放在 PlayerState 是更通用的做法。
 */
UCLASS()
class RPG_API ARPGPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ARPGPlayerState();

	// ~IAbilitySystemInterface 
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~IAbilitySystemInterface

	/** 获取 RPG ASC（类型安全） */
	URPGAbilitySystemComponent* GetRPGAbilitySystemComponent() const { return AbilitySystemComponent; }

	/** 初始化 ASC 的 ActorInfo（绑定 PlayerState 自身作为 Owner，Pawn 作为 Avatar） */
	void InitAbilityActorInfo(APawn* InPawn);

	/** 撤销 AbilitySet（清理授予的内容） */
	void RemoveAbilitySet();

protected:

	/** 创建 AbilitySystemComponent 的子组件（可选，这里直接在构造函数创建） */

	// ========================================================================
	// 核心组件
	// ========================================================================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URPGAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponentForReplication;

	// ========================================================================
	// AbilitySet - 配置要授予的 AttributeSet 和初始化 GE
	// ========================================================================
	// 注意：单个 DefaultAbilitySet 已移除，改为使用 URPGCharacterConfig 全局配置
	// 在 URPGGameInstance::ActiveCharacterConfig 中配置

	// 记录授予的 Handle，用于精确撤销
	// 现在使用 TArray 因为一个 Character 可能授予多个 AbilitySet
	TArray<FRPGAbilitySet_GrantedHandles> GrantedHandlesList;

	/** 根据 CharacterID 初始化技能和属性 */
	void InitAbilitiesAndAttributes(FName CharacterID);

	// ========================================================================
	// Tag 关系映射（可选，在蓝图或 GameData 中配置）
	// ========================================================================
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TObjectPtr<URPGAbilityTagRelationshipMapping> TagRelationshipMapping;
};
