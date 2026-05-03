// Fill out your copyright notice in the Description page of Project Settings.

#include "RPGCharacter.h"
#include "RPGPlayerState.h"
#include "../AbilitySystem/RPGAbilitySystemComponent.h"
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

	// 授予能力（仅在Server或单机有效）
	if (CachedASC && HasAuthority())
	{
		// 方式1：通过 AbilitySet 授予（推荐）
		GiveDefaultAbilitySets();
		
		// 方式2：硬编码授予（临时测试用，后续可删除）
		// GiveTestAbility();
	}
}

void ARPGCharacter::GiveTestAbility()
{
	if (!CachedASC)
	{
		UE_LOG(LogRPG, Error, TEXT("GiveTestAbility: CachedASC is null!"));
		return;
	}

	// 运行时加载 GA_Test 蓝图类（路径根据Content目录结构）
	UClass* TestAbilityClass = LoadObject<UClass>(nullptr, TEXT("/Game/GameplayAbility/GA_Test.GA_Test_C"));
	if (TestAbilityClass)
	{
		// 授予能力
		FGameplayAbilitySpec Spec(TestAbilityClass);
		FGameplayAbilitySpecHandle SpecHandle = CachedASC->GiveAbility(Spec);

		// 获取 GA_Test 的 InputTag 并添加到 DynamicTags
		if (const URPGGameplayAbility* AbilityCDO = Cast<URPGGameplayAbility>(TestAbilityClass->GetDefaultObject()))
		{
			FGameplayTag InputTag = AbilityCDO->GetAbilityInputTag();
			
			if (InputTag.IsValid())
			{
				CachedASC->AddInputTagToAbilitySpec(SpecHandle, InputTag);
			}
			else
			{
				UE_LOG(LogRPG, Error, TEXT("GiveTestAbility: AbilityInputTag is INVALID! Please set Ability Input Tag in GA_Test blueprint."));
			}
		}
		else
		{
			UE_LOG(LogRPG, Error, TEXT("GiveTestAbility: Failed to get AbilityCDO or not URPGGameplayAbility!"));
		}
	}
	else
	{
		UE_LOG(LogRPG, Error, TEXT("GiveTestAbility: Failed to load GA_Test! Check path: /Game/GameplayAbility/GA_Test.GA_Test_C"));
	}
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
}

void ARPGCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Client 端：PlayerState 复制后，初始化 ASC 的 ActorInfo
	if (ARPGPlayerState* PS = GetPlayerState<ARPGPlayerState>())
	{
		PS->InitAbilityActorInfo(this);
	}
}

void ARPGCharacter::GiveDefaultAbilitySets()
{
	if (!CachedASC)
	{
		UE_LOG(LogRPG, Error, TEXT("GiveDefaultAbilitySets: CachedASC is null!"));
		return;
	}

	if (DefaultAbilitySets.Num() == 0)
	{
		UE_LOG(LogRPG, Warning, TEXT("GiveDefaultAbilitySets: DefaultAbilitySets is empty! Please add AbilitySet assets in Blueprint."));
		return;
	}

	// 遍历所有 AbilitySet 并授予
	for (int32 i = 0; i < DefaultAbilitySets.Num(); i++)
	{
		URPGAbilitySet* AbilitySet = DefaultAbilitySets[i].Get();
		
		if (!AbilitySet)
		{
			UE_LOG(LogRPG, Warning, TEXT("GiveDefaultAbilitySets: AbilitySet at index %d is null!"), i);
			continue;
		}

		// 确保 DefaultAbilitySetHandles 有足够空间
		if (DefaultAbilitySetHandles.Num() <= i)
		{
			DefaultAbilitySetHandles.AddDefaulted(i - DefaultAbilitySetHandles.Num() + 1);
		}

		// 授予 AbilitySet
		bool bSuccess = AbilitySet->GiveToAbilitySystem(CachedASC, &DefaultAbilitySetHandles[i]);

		if (!bSuccess)
		{
			UE_LOG(LogRPG, Error, TEXT("GiveDefaultAbilitySets: Failed to grant [%s]"), *GetNameSafe(AbilitySet));
		}
	}
}

void ARPGCharacter::RemoveDefaultAbilitySets()
{
	if (!CachedASC)
	{
		return;
	}

	// 撤销所有授予的 AbilitySets
	for (FRPGAbilitySet_GrantedHandles& Handles : DefaultAbilitySetHandles)
	{
		Handles.TakeFromAbilitySystem(CachedASC);
	}

	DefaultAbilitySetHandles.Reset();
}
