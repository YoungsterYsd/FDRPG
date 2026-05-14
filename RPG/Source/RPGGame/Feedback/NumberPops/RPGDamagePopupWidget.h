// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "RPGDamagePopupWidget.generated.h"

#define UE_API RPGGAME_API

/**
 * URPGDamagePopupWidget（A3-8 v7）
 *
 *   飘字 Widget 父类。BP 子类 WBP_RPG_DamageNumber 需要：
 *     1) 实现 InitDamagePopup BlueprintImplementableEvent —— 根据 Amount/bIsCrit/bIsBlocked 切样式
 *     2) 自带 1.0s 上浮 + fade out 动画（建议 Animation Track 在 BP 里 PlayAnimation）
 *     3) 动画结束后调 RemoveFromParent() 自销毁
 *
 *   样式判定（订阅端 URPGDamageNumberSubsystem 已附 StyleHint 字符串，但建议 BP 自行根据布尔判定）：
 *     - bIsCrit  == true              → 暴击（金 40 + 描边）
 *     - Amount   <  0                 → 治疗（绿 28 + "+"）
 *     - bIsBlocked == true            → 格挡（灰 24 + "BLOCK"）
 *     - 其他                          → 普通（白 28）
 */
UCLASS(MinimalAPI, Abstract, Blueprintable)
class URPGDamagePopupWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/**
	 * 由 URPGDamageNumberSubsystem 在 CreateWidget 后立即调用。
	 * BP 子类实现：根据传入参数切换样式 + 启动动画。
	 *
	 * @param Amount       数值（伤害正、治疗负）
	 * @param bIsCrit      是否暴击
	 * @param bIsBlocked   是否被格挡
	 * @param StyleHint    "Normal" / "Crit" / "Healing" / "Blocked"（订阅端预先判好的字符串提示）
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Damage Popup")
	UE_API void InitDamagePopup(float Amount, bool bIsCrit, bool bIsBlocked, const FString& StyleHint);

	/**
	 * "PERFECT BLOCK!" 完美格挡飘字入口。
	 * BP 子类可单独实现一份金色描边动画。
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Damage Popup")
	UE_API void InitPerfectBlockPopup(float DamageBlocked);
};

#undef UE_API
