// Fill out your copyright notice in the Description page of Project Settings.

#include "RPGPlayerState.h"
#include "../AbilitySystem/RPGAbilitySystemComponent.h"
#include "../AbilitySystem/RPGAbilityTagRelationshipMapping.h"
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
	}
}
