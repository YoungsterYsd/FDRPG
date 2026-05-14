// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGCombatStateSubsystem.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "RPGGameplayTags.h"
#include "RPGLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGCombatStateSubsystem)

URPGCombatStateSubsystem::URPGCombatStateSubsystem()
{
}

void URPGCombatStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogRPGAbilitySystem, Log, TEXT("[A3-5] URPGCombatStateSubsystem Initialized. StaminaRegenDelay=%.1fs"), StaminaRegenDelay);
}

void URPGCombatStateSubsystem::Deinitialize()
{
	LastCombatTimeMap.Reset();
	Super::Deinitialize();
}

bool URPGCombatStateSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

TStatId URPGCombatStateSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URPGCombatStateSubsystem, STATGROUP_Tickables);
}

void URPGCombatStateSubsystem::Tick(float DeltaTime)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const float Now = World->GetTimeSeconds();

	for (auto It = LastCombatTimeMap.CreateIterator(); It; ++It)
	{
		AActor* Actor = It->Key.Get();
		if (!Actor)
		{
			It.RemoveCurrent();
			continue;
		}

		const float LastTime = It->Value;
		const float Elapsed = Now - LastTime;

		UAbilitySystemComponent* ASC = GetASC(Actor);
		if (!ASC)
		{
			It.RemoveCurrent();
			continue;
		}

		const bool bShouldBeInCombat = (Elapsed < StaminaRegenDelay);
		const bool bCurrentlyInCombat = ASC->HasMatchingGameplayTag(RPGGameplayTags::State_Combat_Active);

		if (bShouldBeInCombat != bCurrentlyInCombat)
		{
			SetCombatTag(ASC, bShouldBeInCombat);
			UE_LOG(LogRPGAbilitySystem, Verbose, TEXT("[A3-5] Combat state for %s -> %s (elapsed=%.2fs)"),
				*Actor->GetName(), bShouldBeInCombat ? TEXT("InCombat") : TEXT("OutOfCombat"), Elapsed);
		}

		// 完全脱战且超过 2 倍延迟 → 从 Map 移除（节省 Tick 开销）
		if (!bShouldBeInCombat && Elapsed > StaminaRegenDelay * 2.0f)
		{
			It.RemoveCurrent();
		}
	}
}

void URPGCombatStateSubsystem::NotifyCombatAction(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetASC(Actor);
	if (!ASC)
	{
		return;
	}

	const float Now = World->GetTimeSeconds();
	LastCombatTimeMap.FindOrAdd(Actor) = Now;

	// 立即加 Tag（避免下一帧 Tick 才生效，导致 Stamina Regen GE 在受击瞬间还在回血）
	if (!ASC->HasMatchingGameplayTag(RPGGameplayTags::State_Combat_Active))
	{
		SetCombatTag(ASC, true);
		UE_LOG(LogRPGAbilitySystem, Log, TEXT("[A3-5] %s entered combat"), *Actor->GetName());
	}
}

bool URPGCombatStateSubsystem::IsInCombat(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}

	const UAbilitySystemComponent* ASC = GetASC(Actor);
	return ASC && ASC->HasMatchingGameplayTag(RPGGameplayTags::State_Combat_Active);
}

void URPGCombatStateSubsystem::ClearCombatState(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	LastCombatTimeMap.Remove(Actor);

	if (UAbilitySystemComponent* ASC = GetASC(Actor))
	{
		if (ASC->HasMatchingGameplayTag(RPGGameplayTags::State_Combat_Active))
		{
			SetCombatTag(ASC, false);
			UE_LOG(LogRPGAbilitySystem, Log, TEXT("[A3-5] %s combat cleared"), *Actor->GetName());
		}
	}
}

UAbilitySystemComponent* URPGCombatStateSubsystem::GetASC(AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}
	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
}

void URPGCombatStateSubsystem::SetCombatTag(UAbilitySystemComponent* ASC, bool bInCombat)
{
	if (!ASC)
	{
		return;
	}

	if (bInCombat)
	{
		ASC->AddLooseGameplayTag(RPGGameplayTags::State_Combat_Active);
	}
	else
	{
		ASC->RemoveLooseGameplayTag(RPGGameplayTags::State_Combat_Active);
	}
}
