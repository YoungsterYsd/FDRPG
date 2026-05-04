// Fill out your copyright notice in the Description page of Project Settings.

#include "RPGCharacter.h"
#include "RPGPlayerState.h"
#include "../AbilitySystem/Core/RPGAbilitySystemComponent.h"
#include "../System/RPGGameInstance.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"
#include "../System/RPGLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGCharacter)

ARPGCharacter::ARPGCharacter()
{
	// 设置网络复制频率（使用新 API，直接访问 NetUpdateFrequency 已弃用）
	SetNetUpdateFrequency(100.0f);
	SetMinNetUpdateFrequency(33.0f);

	// 确保 Character 在客户端也复制
	bReplicates = true;
}

UAbilitySystemComponent* ARPGCharacter::GetAbilitySystemComponent() const
{
	// 将查询转发给 PlayerState 中的 ASC
	if (ARPGPlayerState* PS = GetPlayerState<ARPGPlayerState>())
	{
		return PS->GetAbilitySystemComponent();
	}
	return nullptr;
}

URPGAbilitySystemComponent* ARPGCharacter::GetRPGAbilitySystemComponent() const
{
	if (!CachedASC)
	{
		if (ARPGPlayerState* PS = GetPlayerState<ARPGPlayerState>())
		{
			CachedASC = PS->GetRPGAbilitySystemComponent();
		}
	}
	return CachedASC;
}

void ARPGCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 初始化 CachedASC
	CachedASC = GetRPGAbilitySystemComponent();

	// 注意：能力授予已移至 InitFromCharacterConfig() 中
	// 在 PossessedBy() (Server) 或 OnRep_PlayerState() (Client) 时调用
}

void ARPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// 注意：输入绑定已移至 ARPGPlayerController::SetupInputComponent
	// 此方法留空，或仅调用 Super（如果需要的话）
	// 输入绑定在 PlayerController 中进行，因为 Character 可能会被销毁（如死亡重生）
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ARPGCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Server 端：初始化 ASC 的 ActorInfo
	if (ARPGPlayerState* PS = GetPlayerState<ARPGPlayerState>())
	{
		PS->InitAbilityActorInfo(this);
	}

	// 根据 CharacterID 初始化技能和属性
	InitFromCharacterConfig();
}

void ARPGCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Client 端：PlayerState 复制后，初始化 ASC 的 ActorInfo
	if (ARPGPlayerState* PS = GetPlayerState<ARPGPlayerState>())
	{
		PS->InitAbilityActorInfo(this);
	}

	// 注意：不需要在这里调用 InitFromCharacterConfig()
	// 因为 Server 端初始化的属性和 GE 会通过 GAS 网络复制自动同步到 Client
}

void ARPGCharacter::InitFromCharacterConfig()
{
	// 获取 GameInstance 中的 CharacterConfig
	URPGGameInstance* RPGGameInstance = URPGGameInstance::GetRPGGameInstance(this);
	if (!RPGGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGCharacter::InitFromCharacterConfig: Failed to get URPGGameInstance!"));
		return;
	}

	// 调用 GameInstance 的 InitCharacter 方法
	// 注意：InitCharacter 内部会检查 HasAuthority，只在 Server 端授予 Ability 和应用 GE
	RPGGameInstance->InitCharacter(this);
}
