// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RPGPlayerController.generated.h"

class URPGHeroComponent;
class URPGAbilitySystemComponent;

/**
 * ARPGPlayerController
 *
 *	RPG 玩家控制器基类。
 *	持有 HeroComponent 负责输入绑定，使用自定义 RPGInputComponent 处理 Tag 化输入。
 *	俯视角模式下显示鼠标光标。
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The player controller class used by this project."))
class RPGGAME_API ARPGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ARPGPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~APlayerController interface
	virtual void InitInputSystem() override;
	//~End of APlayerController interface

	// Get the hero component
	URPGHeroComponent* GetHeroComponent() const { return HeroComponent; }

	// Get the ASC from the player state
	URPGAbilitySystemComponent* GetRPGAbilitySystemComponent() const;

protected:

	// The hero component that manages input binding
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG|Input")
	URPGHeroComponent* HeroComponent;
};
