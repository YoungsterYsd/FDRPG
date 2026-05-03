// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "../Input/RPGInputConfig.h"
#include "RPGPlayerController.generated.h"


class UInputMappingContext;
class URPGAbilitySystemComponent;
class URPGInputConfig;
/**
 * ARPGPlayerController
 * 
 * 玩家控制器，负责：
 * 1. 设置输入绑定（从InputConfig读取配置）
 * 2. 将输入事件转发给控制的Character的ASC
 * 3. 在PostProcessInput中处理缓存的输入状态
 * 
 * 输入流程图：
 * [按键] → [EnhancedInput Action] → [ARPGPlayerController::OnInputTagPressed/Released]
 *        → [通过GetPawn获取RPGCharacter] → [调用ASC::AbilityInputTagPressed/Released]
 *        → [PostProcessInput中调用ASC::ProcessAbilityInput]
 */
UCLASS()
class RPG_API ARPGPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ARPGPlayerController();

protected:

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void PostProcessInput(const float DeltaTime, bool bGamePaused) override;
	
	/** 获取控制的RPGCharacter的ASC */
	URPGAbilitySystemComponent* GetRPGAbilitySystemComponent() const;

	/** Input Config 数据资产 - 包含 InputAction 与 InputTag 的映射配置 */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<const URPGInputConfig> InputConfig;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input mapping context setup and InputAction binding */
	virtual void SetupInputComponent() override;

	/** 按下按键时调用：将 InputAction 对应的 InputTag 发送给 ASC */
	UFUNCTION()
	void OnInputTagPressed(UInputAction* InputAction);

	/** 释放按键时调用：通知 ASC 输入结束 */
	UFUNCTION()
	void OnInputTagReleased(UInputAction* InputAction);

	/** 持续按住时调用（用于 WhileInputActive 策略的能力，如移动） */
	UFUNCTION()
	void OnInputTagHeld(UInputAction* InputAction);
};
