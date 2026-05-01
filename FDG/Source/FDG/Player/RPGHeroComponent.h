// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "RPGHeroComponent.generated.h"

class URPGInputConfig;
class URPGCameraComponent;
class URPGAbilitySystemComponent;
class UInputMappingContext;

/**
 * URPGHeroComponent
 *
 *	玩家输入绑定组件，挂在 PlayerController 上。
 *	读取 PawnData 的 InputConfig，将 InputAction 绑定到 Native 回调或 ASC 能力输入。
 *	同时管理 IMC 堆栈和相机模式切换。
 */
UCLASS(Meta = (BlueprintSpawnableComponent))
class RPGGAME_API URPGHeroComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	URPGHeroComponent();

	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	// Initialize player input (reads PawnData's InputConfig and binds actions)
	void InitializePlayerInput(const URPGInputConfig* InputConfig);

	// Whether the hero component has been initialized with input
	bool IsReadyToBindInput() const { return bReadyToBindInput; }

	// Check if the hero component has been initialized with input
	bool IsInputInitialized() const { return bInputInitialized; }

protected:

	// Set up native input bindings (Move, Look, etc.)
	void SetupNativeInputBindings(const URPGInputConfig* InputConfig);

	// Set up ability input bindings (pressed/released -> ASC)
	void SetupAbilityInputBindings(const URPGInputConfig* InputConfig);

	// Add an InputMappingContext to the stack
	void AddInputMappingContext(UInputMappingContext* MappingContext, int32 Priority);

	// Remove an InputMappingContext from the stack
	void RemoveInputMappingContext(UInputMappingContext* MappingContext);

private:

	// Whether input has been initialized
	bool bReadyToBindInput = false;

	// Whether the full input binding process has completed
	bool bInputInitialized = false;

	// Bound ability input action handles (for cleanup)
	TArray<uint32> AbilityInputBindHandles;

	// The input mapping context that was pushed
	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> CurrentInputMappingContext;

	// The priority at which the IMC was pushed
	int32 CurrentIMCPriority = 0;
};
