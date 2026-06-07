// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UserSettings/EnhancedInputUserSettings.h"
#include "PlayerMappableKeySettings.h"
#include "RPGInputUserSettings.generated.h"

#define UE_API RPGGAME_API

/** 
 * URPGInputUserSettings — 玩家按键自定义设置的序列化容器
 *
 * 继承自 UEnhancedInputUserSettings，负责存储玩家在设置菜单中修改的按键绑定。
 * - 所有 UPROPERTY(SaveGame) 标记的属性通过 SaveGame 系统自动持久化到磁盘
 * - ApplySettings() 由 URPGSettingsShared::ApplySettings() 链式调用，使自定义映射全局生效
 * - 支持云存档同步（通过 URPGSettingsShared 的序列化流程）
 *
 * 扩展点：可在此类中添加"长按时间阈值""Toggle vs Hold 偏好"等自定义输入设置属性。
 */
UCLASS(MinimalAPI)
class URPGInputUserSettings : public UEnhancedInputUserSettings
{
	GENERATED_BODY()
public:
	//~ Begin UEnhancedInputUserSettings interface
	UE_API virtual void ApplySettings() override;
	//~ End UEnhancedInputUserSettings interface

	/*
	 * 扩展点：如需添加自定义输入设置属性，在此声明并标记 UPROPERTY(SaveGame)。
	 * 示例：Toggle vs Hold 偏好、长按时间阈值、瞄准灵敏度等。
	 */
};

/**
 * URPGPlayerMappableKeySettings — 单个按键的元数据
 *
 * 附加在每个 UInputAction 资产上（PlayerMappableKeySettings 属性）。
 * 为设置 UI 提供 Tooltip 等展示文本，例如"跳跃""使用物品""打开背包"。
 * 当玩家在设置界面浏览可改键的 Action 列表时，UI 通过 GetTooltipText() 获取说明文字。
 */
UCLASS(MinimalAPI)
class URPGPlayerMappableKeySettings : public UPlayerMappableKeySettings
{
	GENERATED_BODY()
	
public:

	/** Returns the tooltip that should be displayed on the settings screen for this key */
	UE_API const FText& GetTooltipText() const;

protected:
	/** The tooltip that should be associated with this action when displayed on the settings screen */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta=(AllowPrivateAccess=true))
	FText Tooltip = FText::GetEmpty();
};

#undef UE_API
