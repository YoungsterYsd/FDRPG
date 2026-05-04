// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "RPGPlayerController.h"
#include "RPGCharacter.h"
#include "../AbilitySystem/Core/RPGAbilitySystemComponent.h"
#include "../System/RPGLogChannels.h"

ARPGPlayerController::ARPGPlayerController()
{
}

void ARPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

}
void ARPGPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ARPGPlayerController::PostProcessInput(const float DeltaTime, bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);

	// 在输入处理完成后，通知 ASC 处理所有缓存的输入状态
	// 这是 Lyra 架构中的标准做法：
	// 1. SetupPlayerInputComponent 中绑定 InputAction → OnInputTagPressed/Released
	// 2. 按下时调用 ASC::AbilityInputTagPressed/Released(InputTag)  [缓存输入状态到 InputPressedSpecHandles 等]
	// 3. PostProcessInput 中调用 ASC::ProcessAbilityInput(DeltaTime, bGamePaused)  [实际激活能力]
	//
	// 为什么不在 OnInputTagPressed 中直接激活？
	// - 一帧内可能有多个输入事件，统一在 PostProcessInput 中处理可以避免重复激活
	// - 某些能力需要等待输入处理完成后再决定激活（如组合键）
	if (ARPGCharacter* MyChar = GetPawn<ARPGCharacter>())
	{
		if (URPGAbilitySystemComponent* ASC = MyChar->GetRPGAbilitySystemComponent())
		{
			ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
		}
	}
}

//URPGAbilitySystemComponent* ARPGPlayerController::GetRPGAbilitySystemComponent() const
//{
//	if (ARPGPlayerState* RPGPS = GetPlayerState<ARPGPlayerState>())
//	{
//		return RPGPS->GetRPGAbilitySystemComponent();
//	}
//	return nullptr;
//}

URPGAbilitySystemComponent* ARPGPlayerController::GetRPGAbilitySystemComponent() const
{
	// 通过控制的Pawn获取RPGCharacter，再获取ASC
	if (ARPGCharacter* MyChar = GetPawn<ARPGCharacter>())
	{
		return MyChar->GetRPGAbilitySystemComponent();
	}
	return nullptr;
}

void ARPGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 初始化 Enhanced Input Component
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// 从 InputConfig 绑定 InputAction
		if (InputConfig)
		{
			// 遍历 InputConfig 中的 AbilityInputActions 并绑定
			for (const FRPGInputAction& Input : InputConfig->GetAbilityInputActions())
			{
				UInputAction* InputAction = const_cast<UInputAction*>(Input.InputAction);
				const FGameplayTag& InputTag = Input.InputTag;

				if (!InputAction || !InputTag.IsValid())
				{
					UE_LOG(LogRPG, Warning, TEXT("SetupInputComponent: Invalid InputAction or InputTag in InputConfig!"));
					continue;
				}

				// 绑定 Started（按下瞬间，只触发一次）→ OnInputTagPressed
				EnhancedInput->BindAction(InputAction, ETriggerEvent::Started, this, &ARPGPlayerController::OnInputTagPressed, InputAction);

				// 绑定 Completed（正常释放）→ OnInputTagReleased
				EnhancedInput->BindAction(InputAction, ETriggerEvent::Completed, this, &ARPGPlayerController::OnInputTagReleased, InputAction);

				// 绑定 Canceled（被中断，如优先级更高的输入）→ OnInputTagReleased
				EnhancedInput->BindAction(InputAction, ETriggerEvent::Canceled, this, &ARPGPlayerController::OnInputTagReleased, InputAction);
			}
		}
		else
		{
			UE_LOG(LogRPG, Error, TEXT("SetupInputComponent: InputConfig is null! Please assign an InputConfig asset in Blueprint."));
		}
	}
	else
	{
		UE_LOG(LogRPG, Error, TEXT("SetupInputComponent: Failed to initialize EnhancedInputComponent!"));
	}
	
	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			if (DefaultMappingContexts.Num() == 0)
			{
				UE_LOG(LogRPG, Warning, TEXT("SetupInputComponent: DefaultMappingContexts is empty! Please add IMC assets in Blueprint."));
			}
			
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				if (CurrentContext)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
				else
				{
					UE_LOG(LogRPG, Warning, TEXT("SetupInputComponent: Found null MappingContext in DefaultMappingContexts!"));
				}
			}
		}
		else
		{
			UE_LOG(LogRPG, Error, TEXT("SetupInputComponent: EnhancedInputSubsystem is null!"));
		}
	}
}

void ARPGPlayerController::OnInputTagPressed(UInputAction* InputAction)
{
	// 从 InputConfig 中找到 InputAction 对应的 InputTag
	if (!InputConfig)
	{
		UE_LOG(LogRPG, Error, TEXT("OnInputTagPressed: InputConfig is null!"));
		return;
	}

	const FGameplayTag& InputTag = InputConfig->FindInputTagForAction(InputAction);
	if (!InputTag.IsValid())
	{
		UE_LOG(LogRPG, Warning, TEXT("OnInputTagPressed: Invalid InputTag for InputAction [%s]!"), *GetNameSafe(InputAction));
		return;
	}
	
	// 通知 ASC：该 InputTag 被按下了
	if (URPGAbilitySystemComponent* ASC = GetRPGAbilitySystemComponent())
	{
		ASC->AbilityInputTagPressed(InputTag);
	}
	else
	{
		UE_LOG(LogRPG, Error, TEXT("OnInputTagPressed: ASC is null!"));
	}
}

void ARPGPlayerController::OnInputTagReleased(UInputAction* InputAction)
{
	// 从 InputConfig 中找到 InputAction 对应的 InputTag
	if (!InputConfig)
	{
		UE_LOG(LogRPG, Error, TEXT("OnInputTagReleased: InputConfig is null!"));
		return;
	}

	const FGameplayTag& InputTag = InputConfig->FindInputTagForAction(InputAction);
	if (!InputTag.IsValid())
	{
		UE_LOG(LogRPG, Warning, TEXT("OnInputTagReleased: Invalid InputTag for InputAction [%s]!"), *GetNameSafe(InputAction));
		return;
	}
	
	// 通知 ASC：该 InputTag 被释放了
	if (URPGAbilitySystemComponent* ASC = GetRPGAbilitySystemComponent())
	{
		ASC->AbilityInputTagReleased(InputTag);
	}
	else
	{
		UE_LOG(LogRPG, Error, TEXT("OnInputTagReleased: ASC is null!"));
	}
}

void ARPGPlayerController::OnInputTagHeld(UInputAction* InputAction)
{
	// 对于 WhileInputActive 策略的能力（如移动），持续按住时需要持续通知 ASC
	// 但通常 ProcessAbilityInput(InputTag, true) 已经将 InputCount > 0，
	// ASC 的 ProcessAbilityInput() 会在每帧尝试激活，所以这里可以留空
	// 如果需要在 Held 时执行特殊逻辑，可以在此添加
}
