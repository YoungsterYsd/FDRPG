// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGGameplayAbility_Block.h"
#include "RPGLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGGameplayAbility_Block)

URPGGameplayAbility_Block::URPGGameplayAbility_Block(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// A3 阶段空壳：标记为 Abstract，不允许实例化激活；A5 由具体 BP 子类继承并实现。
}

void URPGGameplayAbility_Block::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// A3 空壳：Print Log 占位，验证 GA 注册链路即可。A5 阶段实现完整逻辑：
	//  1. ASC->ApplyGameplayEffectToSelf(GE_Block_State, SetByCaller: BlockDamageReduction / BlockStaminaConsume)
	//  2. ASC->AddLooseGameplayTag(RPG.State.Block.PerfectBlocking)（窗口期）
	//  3. World->GetTimerManager().SetTimer(WindowTimer, this, RemovePerfectBlocking, PerfectBlockWindow)
	//  4. UGameplayMessageSubsystem.RegisterListener(RPG.Message.Block.PerfectTriggered) → 完美格挡奖励
	UE_LOG(LogRPGAbilitySystem, Log, TEXT("[A3 stub] URPGGameplayAbility_Block::ActivateAbility called. PerfectWindow=%.2f / DmgRed=%.2f / StaminaCost=%.2f / BrokenStun=%.2f"),
		PerfectBlockWindow, BlockDamageReduction, BlockStaminaConsumeRate, BlockBrokenStunDuration);

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void URPGGameplayAbility_Block::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	UE_LOG(LogRPGAbilitySystem, Log, TEXT("[A3 stub] URPGGameplayAbility_Block::EndAbility called. bWasCancelled=%d"), bWasCancelled ? 1 : 0);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
