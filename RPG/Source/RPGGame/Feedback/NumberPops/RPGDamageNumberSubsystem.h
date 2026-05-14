// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "RPGDamageNumberSubsystem.generated.h"

#define UE_API RPGGAME_API

class AActor;
class UClass;
class UUserWidget;
class URPGDamagePopupWidget;
struct FRPGDamagePopupMessage;
struct FRPGPerfectBlockMessage;

/**
 * URPGDamageNumberSubsystem（A3-8 v7）
 *
 *   订阅 GameplayMessage：
 *     RPG.Message.Damage.Popup            → 创建 WBP_RPG_DamageNumber Widget（4 样式）
 *     RPG.Message.Block.PerfectTriggered  → 播 "PERFECT BLOCK!" 飘字
 *
 *   工作流：
 *     1) Initialize 时注册 2 条 Message 监听
 *     2) 收到消息后用 PlayerController.ProjectWorldLocationToScreen 投影 HitLocation
 *     3) CreateWidget(DamageNumberWidgetClass) + AddToViewport + SetPositionInViewport
 *     4) 调用 BP 子类的 InitDamagePopup / InitPerfectBlockPopup BlueprintImplementableEvent
 *     5) 由 BP Widget 自带的动画播完后 RemoveFromParent 自销毁（C++ 不管理生命周期）
 *
 *   配置：
 *     DamageNumberWidgetClass —— 必须在项目设置或 Subsystem CDO 中指向 WBP_RPG_DamageNumber，
 *     否则只 Print Log，不创建 Widget（避免 A3 阶段编辑器未配资产时崩溃）。
 *
 *   完整链路设计见 14_后续日程与验收清单.md §3.6
 */
UCLASS(MinimalAPI, Config = Game)
class URPGDamageNumberSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UE_API URPGDamageNumberSubsystem();

	// === USubsystem ===
	UE_API virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	UE_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UE_API virtual void Deinitialize() override;

	/**
	 * 飘字 Widget 类（BP 子类继承自 URPGDamagePopupWidget）。
	 * A3-8 编辑器步骤：在 Project Settings → Default URPGDamageNumberSubsystem CDO 设此项为 WBP_RPG_DamageNumber，
	 * 或者在 DefaultGame.ini 里写：
	 *   [/Script/RPGGame.RPGDamageNumberSubsystem]
	 *   DamageNumberWidgetClass=/Game/UI/FeedBack/WBP_RPG_DamageNumber.WBP_RPG_DamageNumber_C
	 *
	 * 为空时只 Print Log，不影响其他系统。
	 */
	UPROPERTY(Config, EditDefaultsOnly, Category = "RPG|Damage Popup", meta = (MetaClass = "/Script/RPGGame.RPGDamagePopupWidget"))
	TSoftClassPtr<UUserWidget> DamageNumberWidgetClass;

private:

	/** Message 订阅句柄。 */
	FGameplayMessageListenerHandle DamagePopupListenerHandle;
	FGameplayMessageListenerHandle PerfectBlockListenerHandle;

	/** 收到伤害飘字消息时调用。 */
	void OnDamagePopupReceived(FGameplayTag Channel, const FRPGDamagePopupMessage& Payload);

	/** 收到完美格挡消息时调用。 */
	void OnPerfectBlockReceived(FGameplayTag Channel, const FRPGPerfectBlockMessage& Payload);

	/** 解析 SoftClassPtr → UClass*。 */
	UClass* ResolveWidgetClass() const;

	/** 通用 Widget 创建/投影封装。 */
	URPGDamagePopupWidget* SpawnPopupWidget(const FVector& WorldLocation, AActor* TargetFallback) const;
};

#undef UE_API

