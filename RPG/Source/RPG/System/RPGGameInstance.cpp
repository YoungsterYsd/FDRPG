// Fill out your copyright notice in the Description page of Project Settings.

#include "RPGGameInstance.h"
#include "../GamePlay/RPGCharacter.h"
#include "../GamePlay/RPGPlayerState.h"
#include "../AbilitySystem/AbilitySet/RPGAbilitySet.h"
#include "../AbilitySystem/Core/RPGAbilitySystemComponent.h"
#include "../AbilitySystem/AttributeSystem/RPGAttributeSet.h"
#include "../System/RPGCharacterConfig.h"
#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGGameInstance)

URPGGameInstance::URPGGameInstance()
{
}

void URPGGameInstance::SwitchCharacterConfig(URPGCharacterConfig* NewConfig)
{
	// 切换到新的角色配置，需要先清理旧配置授予的所有 Ability
	if (!NewConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("SwitchCharacterConfig: NewConfig is null!"));
		return;
	}

	// 1. 清除旧配置授予的所有 Ability
	for (auto& Pair : ActorGrantedHandles)
	{
		if (!Pair.Key.IsValid())
		{
			continue;
		}

		AActor* Actor = Pair.Key.Get();
		if (!Actor)
		{
			continue;
		}

		// 获取 Actor 的 ASC
		UAbilitySystemComponent* ASC = nullptr;
		if (ARPGCharacter* Character = Cast<ARPGCharacter>(Actor))
		{
			ASC = Character->GetAbilitySystemComponent();
		}
		else if (ARPGPlayerState* PS = Cast<ARPGPlayerState>(Actor))
		{
			ASC = PS->GetAbilitySystemComponent();
		}

		if (ASC)
		{
			// 撤销所有授予的 AbilitySet
			for (FRPGAbilitySet_GrantedHandles& Handles : Pair.Value)
			{
				Handles.TakeFromAbilitySystem(Cast<URPGAbilitySystemComponent>(ASC));
			}
		}
	}

	// 清空记录
	ActorGrantedHandles.Empty();

	// 2. 设置新的配置
	ActiveCharacterConfig = NewConfig;

	UE_LOG(LogTemp, Log, TEXT("URPGGameInstance::SwitchCharacterConfig: Switched to new config"));
}

URPGGameInstance* URPGGameInstance::GetRPGGameInstance(UObject* WorldContextObject)
{
	// 全局单例访问：通过 WorldContextObject 获取当前 GameInstance
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	return Cast<URPGGameInstance>(World->GetGameInstance());
}

void URPGGameInstance::InitCharacter(ARPGCharacter* Character)
{
	// 初始化角色：授予技能集 + 初始化属性
	// 只在 Server 端执行，Client 通过 GAS 网络复制自动同步
	
	if (!Character)
	{
		UE_LOG(LogTemp, Error, TEXT("InitCharacter: Character is null!"));
		return;
	}

	if (!ActiveCharacterConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("InitCharacter: ActiveCharacterConfig is null!"));
		return;
	}

	// 获取 CharacterID（用于查找配置数据）
	FName CharacterID = Character->GetCharacterID();

	// 获取需要授予的 AbilitySets
	TArray<URPGAbilitySet*> AbilitySets = ActiveCharacterConfig->GetAbilitySetsForCharacter(CharacterID);

	// 获取 ASC
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("URPGGameInstance::InitCharacter: ASC is null for Character [%s]!"), *Character->GetName());
		return;
	}

	URPGAbilitySystemComponent* RPGASC = Cast<URPGAbilitySystemComponent>(ASC);
	if (!RPGASC)
	{
		UE_LOG(LogTemp, Error, TEXT("URPGGameInstance::InitCharacter: Failed to cast to URPGAbilitySystemComponent!"));
		return;
	}

	// 只在 Server 端授予 Ability 和应用 GE（Client 通过 GAS 网络复制自动同步）
	if (Character->HasAuthority())
	{
		// 记录此 Character 的 Handles
		TArray<FRPGAbilitySet_GrantedHandles>& HandlesForActor = ActorGrantedHandles.FindOrAdd(Character);

		// 授予所有 AbilitySets
		for (URPGAbilitySet* AbilitySet : AbilitySets)
		{
			if (!AbilitySet)
			{
				continue;
			}

			// 添加新的 Handles 记录
			HandlesForActor.AddDefaulted();
			FRPGAbilitySet_GrantedHandles& NewHandles = HandlesForActor.Last();

			bool bSuccess = AbilitySet->GiveToAbilitySystem(RPGASC, &NewHandles);
			if (!bSuccess)
			{
				UE_LOG(LogTemp, Error, TEXT("URPGGameInstance::InitCharacter: Failed to grant AbilitySet [%s]"), *GetNameSafe(AbilitySet));
			}
		}

		// 应用属性初始化 - 直接设置属性值（稳定方案，无需创建GE资产）
		const FRPGAttributeInitTableRow* InitData = ActiveCharacterConfig->GetAttributeInitData(CharacterID);
		if (InitData)
		{
			// 直接设置属性值 - 使用GAS标准接口 SetNumericAttributeBase
			// 这些设置会在Server端执行，并自动网络复制到Client
			
			// 1. 基础属性 (Basic)
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetHp_BasicAttribute(), InitData->Hp_Basic);
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetAtk_BasicAttribute(), InitData->Atk_Basic);
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetDef_BasicAttribute(), InitData->Def_Basic);
			
			// 2. 额外属性 (PostAdd)
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetHp_PostAddAttribute(), InitData->Hp_PostAdd);
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetAtk_PostAddAttribute(), InitData->Atk_PostAdd);
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetDef_PostAddAttribute(), InitData->Def_PostAdd);
			
			// 3. 百分比属性 (Mul)
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetHp_MulAttribute(), InitData->Hp_Mul);
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetAtk_MulAttribute(), InitData->Atk_Mul);
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetDef_MulAttribute(), InitData->Def_Mul);
			
			// 4. 战斗属性 - 暴击
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetCritAttribute(), InitData->Crit);
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetCritDmgAttribute(), InitData->CritDmg);
			
			// 5. 战斗属性 - 防御穿透
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetIgnDefAttribute(), InitData->IgnDef);
			
			// 6. 战斗属性 - 充能
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetPowerRegenRateAttribute(), InitData->PowerRegenRate);
			
			// 7. 战斗属性 - 击破
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetBreakBonusAttribute(), InitData->BreakBonus);
			
			// 8. 战斗属性 - 伤害修正
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetDmg_MulAttribute(), InitData->Dmg_Mul);
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetDmgRed_MulAttribute(), InitData->DmgRed_Mul);
			
			// 9. 战斗属性 - 吸血
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetHealthStealAttribute(), InitData->HealthSteal);
			
			// 10. 特殊属性 - 速度
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetNormalSkillSpeedAttribute(), InitData->NormalSkillSpeed);
			
			// 11. 特殊属性 - 移动
			ASC->SetNumericAttributeBase(URPGAttributeSet::GetMoveSpeedAttribute(), InitData->MoveSpeed);
			
			UE_LOG(LogTemp, Verbose, TEXT("InitCharacter: Attributes initialized for Character [%s]"), 
				*Character->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("InitCharacter: Failed to get AttributeInitData for CharacterID [%s]"), *CharacterID.ToString());
		}
	}
}
