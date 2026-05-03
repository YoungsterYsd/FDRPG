// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "InputActionValue.h"
#include "RPGCharacter.generated.h"

class URPGPawnExtensionComponent;
class URPGCameraComponent;
class URPGAbilitySystemComponent;

/**
 * ARPGCharacter
 *
 *	俯视角 RPG 角色基类。
 *	持有 PawnExtensionComponent（初始化状态机）和 CameraComponent（相机系统），
 *	通过 IAbilitySystemInterface 提供 ASC 访问。
 *
 *	角色朝向由移动方向决定（bOrientRotationToMovement = true），
 *	相机由 RPGCameraComponent + CameraModeStack 驱动。
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base character class used by this project."))
class RPGGAME_API ARPGCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	ARPGCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;
	//~End of AActor interface

	//~APawn interface
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;
	//~End of APawn interface

	//~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface

	// Get the pawn extension component
	URPGPawnExtensionComponent* GetPawnExtensionComponent() const { return PawnExtensionComponent; }

	// Get the camera component
	URPGCameraComponent* GetCameraComponent() const { return CameraComponent; }

	// Get the ASC from PlayerState
	URPGAbilitySystemComponent* GetRPGAbilitySystemComponent() const;

	// Movement input handler for IA_Move (arrow keys)
	void Move(const FInputActionValue& InputActionValue);

protected:

	// Called when the character is possessed
	void OnAbilitySystemInitialized();

	// Called when the character is unpossessed or destroyed
	void OnAbilitySystemUninitialized();

	// Initialize the pawn data from the AssetManager
	void InitPawnData();

	// Handle death (P2 full implementation, P1 just logs)
	virtual void OnDeath();

	// Called when the death process starts (P2)
	UFUNCTION(BlueprintCallable, Category = "RPG|Character")
	virtual void OnDeathStarted();

	// Called when the death process finishes (P2)
	UFUNCTION(BlueprintCallable, Category = "RPG|Character")
	virtual void OnDeathFinished();

	// Reset character for respawn (P8 settlement phase)
	UFUNCTION(BlueprintCallable, Category = "RPG|Character")
	virtual void ResetCharacter();

	// Called when movement mode changes
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

protected:

	// Pawn extension component - manages initialization state machine
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG|Character")
	URPGPawnExtensionComponent* PawnExtensionComponent;

	// Camera component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG|Camera")
	URPGCameraComponent* CameraComponent;

	// Maximum walk speed (tunable for gameplay feel)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Movement")
	float MaxWalkSpeed = 600.0f;

	// Maximum acceleration
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Movement")
	float MaxAcceleration = 2048.0f;

	// Braking deceleration when walking
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Movement")
	float BrakingDecelerationWalking = 2048.0f;

	// Ground friction when braking
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Movement")
	float BrakingFriction = 8.0f;

private:

	// Whether we've initialized the ability system
	bool bAbilitySystemInitialized = false;
};
