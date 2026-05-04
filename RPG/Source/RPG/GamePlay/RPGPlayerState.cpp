// Fill out your copyright notice in the Description page of Project Settings.

#include "RPGPlayerState.h"
#include "../AbilitySystem/Core/RPGAbilitySystemComponent.h"
#include "../AbilitySystem/Core/RPGAbilityTagRelationshipMapping.h"
#include "../AbilitySystem/AbilitySet/RPGAbilitySet.h"
#include "../System/RPGGameInstance.h"
#include "../System/RPGCharacterConfig.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGPlayerState)

ARPGPlayerState::ARPGPlayerState()
{
	// 设置网络复制频率（PlayerState 默认复制率较高，适合能力状态同步）
	SetNetUpdateFrequency(100.0f);

	// 创建 AbilitySystemComponent
	AbilitySystemComponent = CreateDefaultSubobject<URPGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	// 设置复制（网络多人游戏需要）
	AbilitySystemComponent->SetIsReplicated(true);
	// 混合复制模式：PlayerState 上的 ASC 使用 Mixed 模式
	// （OwnerActor 是 PlayerState，AvatarActor 是 Pawn）
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// 为了兼容 UE 内置的网络复制，同时赋值给 AbilitySystemComponentForReplication
	// （某些引擎版本需要这个成员变量才能正确复制）
	AbilitySystemComponentForReplication = AbilitySystemComponent;

	// 注意：AttributeSet 不再在这里创建
	// 而是通过 DefaultAbilitySet 配置，在 InitAbilityActorInfo 中授予
}

UAbilitySystemComponent* ARPGPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ARPGPlayerState::InitAbilityActorInfo(APawn* InPawn)
{
	if (AbilitySystemComponent && InPawn)
	{
		// 绑定 ASC 的 Owner（PlayerState）和 Avatar（Pawn）
		// - OwnerActor：网络复制的"主人"，通常是 PlayerState
		// - AvatarActor：能力的实际执行者，通常是 Pawn/Character
		AbilitySystemComponent->InitAbilityActorInfo(this, InPawn);

		// 注意：技能和属性初始化已移至 InitAbilitiesAndAttributes
		// 由 Character::InitFromCharacterConfig() -> GameInstance::InitCharacter() 调用
	}
}

void ARPGPlayerState::InitAbilitiesAndAttributes(FName CharacterID)
{
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGPlayerState::InitAbilitiesAndAttributes: AbilitySystemComponent is null!"));
		return;
	}

	// 获取 GameInstance 中的 CharacterConfig
	URPGGameInstance* RPGGameInstance = URPGGameInstance::GetRPGGameInstance(this);
	if (!RPGGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGPlayerState::InitAbilitiesAndAttributes: Failed to get URPGGameInstance!"));
		return;
	}

	URPGCharacterConfig* Config = RPGGameInstance->ActiveCharacterConfig;
	if (!Config)
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGPlayerState::InitAbilitiesAndAttributes: ActiveCharacterConfig is not set!"));
		return;
	}

	// 1. 授予通用技能集
	for (URPGAbilitySet* AbilitySet : Config->GenericAbilitySets)
	{
		if (!AbilitySet)
		{
			continue;
		}

		// 添加一个新的 Handles 记录
		GrantedHandlesList.AddDefaulted();
		FRPGAbilitySet_GrantedHandles& NewHandles = GrantedHandlesList.Last();

		bool bSuccess = AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, &NewHandles);
		if (!bSuccess)
		{
			UE_LOG(LogTemp, Error, TEXT("ARPGPlayerState::InitAbilitiesAndAttributes: Failed to grant GenericAbilitySet [%s]"), *GetNameSafe(AbilitySet));
		}
	}

	// 2. 使用 CharacterID 查找并授予特定技能集
	TArray<URPGAbilitySet*> CharacterAbilitySets = Config->GetAbilitySetsForCharacter(CharacterID);
	for (URPGAbilitySet* AbilitySet : CharacterAbilitySets)
	{
		if (!AbilitySet)
		{
			continue;
		}

		// 添加一个新的 Handles 记录
		GrantedHandlesList.AddDefaulted();
		FRPGAbilitySet_GrantedHandles& NewHandles = GrantedHandlesList.Last();

		bool bSuccess = AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, &NewHandles);
		if (!bSuccess)
		{
			UE_LOG(LogTemp, Error, TEXT("ARPGPlayerState::InitAbilitiesAndAttributes: Failed to grant CharacterAbilitySet [%s]"), *GetNameSafe(AbilitySet));
		}
	}

	// 3. 使用 CharacterID 在 AttributeInitTable 中查找对应行，初始化属性
	const FRPGAttributeInitTableRow* InitData = Config->GetAttributeInitData(CharacterID);
	if (InitData)
	{
		// TODO: 应用属性初始化数据到 ASC
		// 这需要根据你的属性系统实现
		UE_LOG(LogTemp, Log, TEXT("ARPGPlayerState::InitAbilitiesAndAttributes: Initializing attributes for CharacterID [%s]"), *CharacterID.ToString());
	}
}

void ARPGPlayerState::RemoveAbilitySet()
{
	if (AbilitySystemComponent)
	{
		// 撤销所有授予的 AbilitySets
		for (FRPGAbilitySet_GrantedHandles& Handles : GrantedHandlesList)
		{
			Handles.TakeFromAbilitySystem(AbilitySystemComponent);
		}

		GrantedHandlesList.Reset();
	}
}
