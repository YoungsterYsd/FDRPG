// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGPlayerController.h"
#include "Player/RPGHeroComponent.h"
#include "Input/RPGInputComponent.h"
#include "AbilitySystem/RPGAbilitySystemComponent.h"
#include "Player/RPGPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGPlayerController)

ARPGPlayerController::ARPGPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HeroComponent = CreateDefaultSubobject<URPGHeroComponent>(TEXT("HeroComponent"));
	bShowMouseCursor = true; // 俯视角需要显示鼠标光标
}

void ARPGPlayerController::InitInputSystem()
{
	// 在 Super 之前创建 RPGInputComponent，替换默认的 UInputComponent
	if (!InputComponent)
	{
		InputComponent = NewObject<URPGInputComponent>(this);
	}

	Super::InitInputSystem();
}

URPGAbilitySystemComponent* ARPGPlayerController::GetRPGAbilitySystemComponent() const
{
	if (const ARPGPlayerState* RPGPS = GetPlayerState<ARPGPlayerState>())
	{
		return RPGPS->GetRPGAbilitySystemComponent();
	}
	return nullptr;
}
