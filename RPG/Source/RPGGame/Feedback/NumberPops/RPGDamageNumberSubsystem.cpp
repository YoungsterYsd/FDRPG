// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGDamageNumberSubsystem.h"
#include "RPGDamagePopupWidget.h"
#include "Blueprint/UserWidget.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Messages/RPGDamagePopupMessage.h"
#include "Messages/RPGBlockMessage.h"
#include "RPGGameplayTags.h"
#include "RPGLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGDamageNumberSubsystem)

URPGDamageNumberSubsystem::URPGDamageNumberSubsystem()
{
}

bool URPGDamageNumberSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// 仅在 Game / PIE World 创建，避免 Editor 工具/预览 World 早于 GameInstance 创建时调用 GameplayMessageSubsystem::Get() 崩溃
	if (const UWorld* World = Cast<UWorld>(Outer))
	{
		const EWorldType::Type WT = World->WorldType;
		return (WT == EWorldType::Game) || (WT == EWorldType::PIE) || (WT == EWorldType::GamePreview);
	}
	return false;
}

void URPGDamageNumberSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UGameInstance* GI = World->GetGameInstance();
	UGameplayMessageSubsystem* GMS = GI ? GI->GetSubsystem<UGameplayMessageSubsystem>() : nullptr;
	if (!GMS)
	{
		UE_LOG(LogRPGAbilitySystem, Warning, TEXT("[A3-8] URPGDamageNumberSubsystem: GameplayMessageSubsystem not available yet for World '%s' (Type=%d). Listeners will not register."),
			*World->GetName(), static_cast<int32>(World->WorldType));
		return;
	}

	DamagePopupListenerHandle = GMS->RegisterListener(
		RPGGameplayTags::Message_Damage_Popup,
		this,
		&URPGDamageNumberSubsystem::OnDamagePopupReceived);

	PerfectBlockListenerHandle = GMS->RegisterListener(
		RPGGameplayTags::Message_Block_PerfectTriggered,
		this,
		&URPGDamageNumberSubsystem::OnPerfectBlockReceived);

	UE_LOG(LogRPGAbilitySystem, Log, TEXT("[A3-8] URPGDamageNumberSubsystem: subscribed to Message.Damage.Popup + Message.Block.PerfectTriggered (World=%s)"),
		*World->GetName());
}

void URPGDamageNumberSubsystem::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			if (UGameplayMessageSubsystem* GMS = GI->GetSubsystem<UGameplayMessageSubsystem>())
			{
				GMS->UnregisterListener(DamagePopupListenerHandle);
				GMS->UnregisterListener(PerfectBlockListenerHandle);
			}
		}
	}
	Super::Deinitialize();
}

UClass* URPGDamageNumberSubsystem::ResolveWidgetClass() const
{
	if (DamageNumberWidgetClass.IsNull())
	{
		return nullptr;
	}

	UClass* Loaded = DamageNumberWidgetClass.Get();
	if (!Loaded)
	{
		Loaded = DamageNumberWidgetClass.LoadSynchronous();
	}
	return Loaded;
}

URPGDamagePopupWidget* URPGDamageNumberSubsystem::SpawnPopupWidget(const FVector& WorldLocation, AActor* TargetFallback) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	UClass* WidgetClass = ResolveWidgetClass();
	if (!WidgetClass)
	{
		UE_LOG(LogRPGAbilitySystem, Warning,
			TEXT("[A3-8] SpawnPopupWidget: DamageNumberWidgetClass is null. Set it in DefaultGame.ini → [/Script/RPGGame.RPGDamageNumberSubsystem] DamageNumberWidgetClass=/Game/UI/.../WBP_RPG_DamageNumber.WBP_RPG_DamageNumber_C  (SoftPath='%s')"),
			*DamageNumberWidgetClass.ToString());
		return nullptr;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC || !PC->GetLocalPlayer())
	{
		UE_LOG(LogRPGAbilitySystem, Warning, TEXT("[A3-8] SpawnPopupWidget: no LocalPlayerController available."));
		return nullptr;
	}

	URPGDamagePopupWidget* Widget = CreateWidget<URPGDamagePopupWidget>(PC, WidgetClass);
	if (!Widget)
	{
		UE_LOG(LogRPGAbilitySystem, Error,
			TEXT("[A3-8] SpawnPopupWidget: CreateWidget failed for class '%s'. Make sure the BP inherits from URPGDamagePopupWidget (not UUserWidget)."),
			*WidgetClass->GetName());
		return nullptr;
	}

	Widget->AddToViewport(/*ZOrder=*/ 100);

	// 把 3D HitLocation（或 Target Actor 位置）投影到屏幕坐标
	FVector2D ScreenPos(0.0f, 0.0f);
	const FVector ProjectFrom = !WorldLocation.IsNearlyZero()
		? WorldLocation
		: (TargetFallback ? TargetFallback->GetActorLocation() + FVector(0, 0, 80.0f) : FVector::ZeroVector);

	const bool bProjected = PC->ProjectWorldLocationToScreen(ProjectFrom, ScreenPos, /*bPlayerViewportRelative=*/ true);
	if (bProjected)
	{
		Widget->SetPositionInViewport(ScreenPos, /*bRemoveDPIScale=*/ false);
	}

	UE_LOG(LogRPGAbilitySystem, Verbose,
		TEXT("[A3-8] SpawnPopupWidget: created '%s' at WorldLoc=%s → ScreenPos=%s (Projected=%s)"),
		*WidgetClass->GetName(), *ProjectFrom.ToCompactString(), *ScreenPos.ToString(),
		bProjected ? TEXT("true") : TEXT("false"));

	return Widget;
}

void URPGDamageNumberSubsystem::OnDamagePopupReceived(FGameplayTag Channel, const FRPGDamagePopupMessage& Payload)
{
	const TCHAR* StyleName = TEXT("Normal");
	if (Payload.bIsCrit)            { StyleName = TEXT("Crit"); }
	else if (Payload.Amount < 0.0f) { StyleName = TEXT("Healing"); }
	else if (Payload.bIsBlocked)    { StyleName = TEXT("Blocked"); }

	UE_LOG(LogRPGAbilitySystem, Log,
		TEXT("[A3-8] DamagePopup: style=%s amount=%.1f IsCrit=%d IsBlocked=%d Target=%s Loc=%s"),
		StyleName, Payload.Amount,
		Payload.bIsCrit ? 1 : 0, Payload.bIsBlocked ? 1 : 0,
		*GetNameSafe(Payload.Target),
		*Payload.HitLocation.ToCompactString());

	AActor* TargetActor = Cast<AActor>(Payload.Target);
	if (URPGDamagePopupWidget* Widget = SpawnPopupWidget(Payload.HitLocation, TargetActor))
	{
		Widget->InitDamagePopup(Payload.Amount, Payload.bIsCrit, Payload.bIsBlocked, FString(StyleName));
	}
}

void URPGDamageNumberSubsystem::OnPerfectBlockReceived(FGameplayTag Channel, const FRPGPerfectBlockMessage& Payload)
{
	UE_LOG(LogRPGAbilitySystem, Log,
		TEXT("[A3-8] PerfectBlock: target=%s damageBlocked=%.1f srcTags=%s loc=%s"),
		*GetNameSafe(Payload.Target),
		Payload.DamageBlocked,
		*Payload.AttackSourceTags.ToString(),
		*Payload.HitLocation.ToCompactString());

	AActor* TargetActor = Cast<AActor>(Payload.Target);
	if (URPGDamagePopupWidget* Widget = SpawnPopupWidget(Payload.HitLocation, TargetActor))
	{
		Widget->InitPerfectBlockPopup(Payload.DamageBlocked);
	}
}
