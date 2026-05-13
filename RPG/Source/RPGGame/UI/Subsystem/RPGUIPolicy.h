// Copyright RPG Demo Project. Derived from CommonGame.

#pragma once

#include "GameUIPolicy.h"

#include "RPGUIPolicy.generated.h"

/**
 * URPGUIPolicy
 *
 * RPG 项目专用的 UI Policy 基类，主要修复 CommonGame UGameUIPolicy 的一个 Editor 体验问题：
 *
 *  原版 UGameUIPolicy::GetWorld() 实现是：
 *      return GetOwningUIManager()->GetGameInstance()->GetWorld();
 *  其中 GetOwningUIManager() 是 CastChecked<UGameUIManagerSubsystem>(GetOuter())。
 *
 *  当用户在 Content Browser 创建/编辑一个 GameUIPolicy 派生蓝图时，CDO 的 Outer 是 Package 而不是
 *  Subsystem，于是任何触发 PostTransacted → GetWorld() 的操作（例如点击 LayoutClass 下拉框）
 *  都会触发 Fatal "Cast of Package /Game/UI/B_RPG_UIPolicy to GameUIManagerSubsystem failed"。
 *
 *  这里 override GetWorld() 做 IsTemplate/IsCDO 双重检查，让 CDO 编辑时安全返回 nullptr，
 *  运行时（Outer 是 Subsystem）继续走原 Lyra/CommonGame 路径不变。
 */
UCLASS(Abstract, Blueprintable, Within = GameUIManagerSubsystem)
class RPGGAME_API URPGUIPolicy : public UGameUIPolicy
{
	GENERATED_BODY()

public:
	//~ UObject
	virtual UWorld* GetWorld() const override;
	//~ End UObject
};
