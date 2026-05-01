// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "RPGPawnExtensionComponent.generated.h"

class URPGPawnData;
class URPGAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRPGPawnReadyToInitialize);

/**
 * URPGPawnExtensionComponent
 *
 *	Pawn 初始化状态机组件，挂载到所有 Pawn 上。
 *	管理 PawnData 访问、ASC 初始化和输入绑定。
 *
 *	初始化状态机流程：
 *	  Spawned -> DataAvailable -> DataInitialized -> GameplayReady
 */
UCLASS(Meta = (BlueprintSpawnableComponent))
class RPGGAME_API URPGPawnExtensionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	URPGPawnExtensionComponent();

	//~UActorComponent interface
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~End of UActorComponent interface

	// Returns the pawn extension component if found on the specified actor
	static URPGPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor);

	// Get the current initialization state
	FGameplayTag GetInitState() const { return InitState; }

	// Set the pawn data and start the initialization process
	void SetPawnData(const URPGPawnData* InPawnData);

	// Get the pawn data
	const URPGPawnData* GetPawnData() const { return PawnData; }

	// Get the ability system component from the pawn's player state
	URPGAbilitySystemComponent* GetAbilitySystemComponent() const;

	// Called when the pawn is ready to initialize (all data available)
	UPROPERTY(BlueprintAssignable, Category = "Pawn")
	FRPGPawnReadyToInitialize OnPawnReadyToInitialize;

	// Called when the pawn data has been set
	UPROPERTY(BlueprintAssignable, Category = "Pawn")
	FRPGPawnReadyToInitialize OnPawnDataChanged;

	// Check if the pawn is in the GameplayReady state
	bool IsReadyToInitialize() const;

	// Called when the controller changes (possess/unpossess)
	void HandleControllerChanged();

	// Called when the player state is replicated/initialized
	void OnPlayerStateChanged();

	// Setup player input component for this pawn
	void SetupPlayerInputComponent();

	// Check if this pawn is controlled by a local player
	bool IsLocalPlayerController() const;

protected:

	// Update the initialization state machine, returns true if state changed
	bool UpdateInitState(float DeltaTime);

	// Check if we can transition to the specified state
	bool CanChangeInitState(FGameplayTag DesiredState) const;

	// Handle a state change
	void HandleInitStateChanged(FGameplayTag OldState, FGameplayTag NewState);

	// Called when the ability system is initialized
	void OnAbilitySystemInitialized();

	// Called when the ability system is uninitialized (called by Character::Reset)
	void OnAbilitySystemUninitialized();

	// The current initialization state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "InitState")
	FGameplayTag InitState;

private:

	// The pawn data for this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn", meta = (AllowPrivateAccess = "true"))
	const URPGPawnData* PawnData = nullptr;

	// Whether the OnPawnReadyToInitialize delegate has been broadcast
	bool bPawnReadyToInitializeBroadcast = false;

	// Whether we've already called OnAbilitySystemInitialized
	bool bAbilitySystemInitialized = false;
};
