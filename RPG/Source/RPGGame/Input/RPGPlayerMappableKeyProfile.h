// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UserSettings/EnhancedInputUserSettings.h"

#include "RPGPlayerMappableKeyProfile.generated.h"

#define UE_API RPGGAME_API

/**
 * URPGPlayerMappableKeyProfile — 按键绑定方案（Profile）
 *
 * 继承自 UEnhancedPlayerMappableKeyProfile，代表一套命名的按键映射方案
 * （如"默认方案""左手模式""手柄模式"）。支持多套方案并存与运行时热切换。
 *
 * - EquipProfile()：方案激活时回调，可在此添加方案切换逻辑
 * - UnEquipProfile()：方案停用时回调
 *
 * 方案切换由 UEnhancedInputUserSettings::SetActiveKeyProfile() 驱动，
 * 所有方案的按键映射数据由 URPGInputUserSettings 统一序列化管理。
 */
UCLASS(MinimalAPI)
class URPGPlayerMappableKeyProfile : public UEnhancedPlayerMappableKeyProfile
{
	GENERATED_BODY()

protected:

	//~ Begin UEnhancedPlayerMappableKeyProfile interface
	UE_API virtual void EquipProfile() override;
	UE_API virtual void UnEquipProfile() override;
	//~ End UEnhancedPlayerMappableKeyProfile interface
};

#undef UE_API
