// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGCharacterWithAbilities.h"

#include "AbilitySystem/Attributes/RPGHealthSet.h"
#include "AbilitySystem/Attributes/RPGPrimaryAttributeSet.h"
#include "AbilitySystem/RPGAbilitySet.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Async/TaskGraphInterfaces.h"
#include "Character/RPGPawnData.h"
#include "Character/RPGPawnExtensionComponent.h"
#include "RPGLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGCharacterWithAbilities)

ARPGCharacterWithAbilities::ARPGCharacterWithAbilities(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<URPGAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	HealthSet = CreateDefaultSubobject<URPGHealthSet>(TEXT("HealthSet"));
	// A3 v7：RPG primary attribute set（URPGCombatSet 已于 A3-3 清理期废弃）
	PrimaryAttributeSet = CreateDefaultSubobject<URPGPrimaryAttributeSet>(TEXT("PrimaryAttributeSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);
}

void ARPGCharacterWithAbilities::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);

	// === 第 1 步：让 PawnExtComp 与本 ASC 双向绑定 ===
	// 走 PawnExtComp::InitializeAbilitySystem 的标准入口，等价于 RPGHeroComponent 的逻辑。
	// 该类主要面向 NPC / 独立 Pawn / 测试场景（无 PlayerState ASC），玩家请使用 ARPGCharacter
	// 配合 GameFeatureAction_AddComponents 注入 HeroComponent 的 Lyra 标准路径。
	URPGPawnExtensionComponent* PawnExtComp = URPGPawnExtensionComponent::FindPawnExtensionComponent(this);
	if (PawnExtComp)
	{
		// 这一步内部会调 ASC->InitAbilityActorInfo(OwnerActor=this, AvatarActor=Pawn=this)
		// 并广播 OnAbilitySystemInitialized → ARPGCharacter::OnAbilitySystemInitialized → HealthComponent 初始化
		PawnExtComp->InitializeAbilitySystem(AbilitySystemComponent, /*OwnerActor=*/ this);
	}
	else
	{
		// 兜底：没有 PawnExtComp 时仍然手动 InitAbilityActorInfo（与原 A3-1 行为一致），但 HealthComponent 不会自动初始化
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	// === 第 2 步：仅 Authority 端 Give AbilitySet ===
	// AbilitySet GiveToAbilitySystem 只能在服务器端（或单机）调用一次，会复制到客户端
	if (HasAuthority())
	{
		// PawnData 由 GameMode/PawnExtComp 在更早阶段（Spawned 状态切换时）注入，
		// 但 ARPGCharacterWithAbilities 通常用于 Editor 直接放置 / Spawn 测试场景，PawnData 可能还没设置。
		// 这里采用最稳路径：从 PawnExtComp 读取 PawnData，若已注入则消费 AbilitySets。
		if (PawnExtComp)
		{
			if (const URPGPawnData* PawnData = PawnExtComp->GetPawnData<URPGPawnData>())
			{
				GrantAbilitySetsFromPawnData(PawnData);
			}
			else
			{
				// 注册一次性回调：等 PawnData 在 PawnExtComp 上就绪后再 Give
				UE_LOG(LogRPGAbilitySystem, Log, TEXT("[A3] %s waiting for PawnData to give AbilitySet (will be polled in NotifyControllerChanged)"),
					*GetNameSafe(this));
			}
		}
	}
}

void ARPGCharacterWithAbilities::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// 第二次尝试 Give AbilitySet —— 此时 PawnData 多半已通过 GameMode 注入
	if (HasAuthority() && AbilitySystemComponent && !bAbilitySetGranted)
	{
		if (URPGPawnExtensionComponent* PawnExtComp = URPGPawnExtensionComponent::FindPawnExtensionComponent(this))
		{
			if (const URPGPawnData* PawnData = PawnExtComp->GetPawnData<URPGPawnData>())
			{
				GrantAbilitySetsFromPawnData(PawnData);
			}
		}
	}
}

void ARPGCharacterWithAbilities::GrantAbilitySetsFromPawnData(const URPGPawnData* InPawnData)
{
	if (!InPawnData || !AbilitySystemComponent || bAbilitySetGranted)
	{
		return;
	}

	int32 GivenCount = 0;
	for (const TObjectPtr<URPGAbilitySet>& AbilitySet : InPawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, /*OutGrantedHandles=*/ nullptr);
			++GivenCount;
		}
	}

	bAbilitySetGranted = true;
	UE_LOG(LogRPGAbilitySystem, Log, TEXT("[A3] %s granted %d AbilitySet(s) from PawnData '%s'"),
		*GetNameSafe(this), GivenCount, *GetNameSafe(InPawnData));
}

UAbilitySystemComponent* ARPGCharacterWithAbilities::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
