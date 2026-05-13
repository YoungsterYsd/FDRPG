// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Camera/RPGCameraAssistInterface.h"
#include "CommonPlayerController.h"
#include "Teams/RPGTeamAgentInterface.h"

#include "RPGPlayerController.generated.h"

#define UE_API RPGGAME_API

struct FGenericTeamId;

class ARPGHUD;
class ARPGPlayerState;
class APawn;
class APlayerState;
class FPrimitiveComponentId;
class IInputInterface;
class URPGAbilitySystemComponent;
class URPGSettingsShared;
class UObject;
class UPlayer;
struct FFrame;

/**
 * ALyraPlayerController
 *
 *	The base player controller class used by this project.
 */
UCLASS(MinimalAPI, Config = Game, Meta = (ShortTooltip = "The base player controller class used by this project."))
class ARPGPlayerController : public ACommonPlayerController, public IRPGCameraAssistInterface, public IRPGTeamAgentInterface
{
	GENERATED_BODY()

public:

	UE_API ARPGPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "RPG|PlayerController")
	UE_API ARPGPlayerState* GetRPGPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "RPG|PlayerController")
	UE_API URPGAbilitySystemComponent* GetRPGAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "RPG|PlayerController")
	UE_API ARPGHUD* GetRPGHUD() const;

	// Call from game state logic to start recording an automatic client replay if ShouldRecordClientReplay returns true
	UFUNCTION(BlueprintCallable, Category = "RPG|PlayerController")
	UE_API bool TryToRecordClientReplay();

	// Call to see if we should record a replay, subclasses could change this
	UE_API virtual bool ShouldRecordClientReplay();

	// Run a cheat command on the server.
	UFUNCTION(Reliable, Server, WithValidation)
	UE_API void ServerCheat(const FString& Msg);

	// Run a cheat command on the server for all players.
	UFUNCTION(Reliable, Server, WithValidation)
	UE_API void ServerCheatAll(const FString& Msg);

	//~AActor interface
	UE_API virtual void PreInitializeComponents() override;
	UE_API virtual void BeginPlay() override;
	UE_API virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UE_API virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~End of AActor interface

	//~AController interface
	UE_API virtual void OnPossess(APawn* InPawn) override;
	UE_API virtual void OnUnPossess() override;
	UE_API virtual void InitPlayerState() override;
	UE_API virtual void CleanupPlayerState() override;
	UE_API virtual void OnRep_PlayerState() override;
	//~End of AController interface

	//~APlayerController interface
	UE_API virtual void ReceivedPlayer() override;
	UE_API virtual void PlayerTick(float DeltaTime) override;
	UE_API virtual void SetPlayer(UPlayer* InPlayer) override;
	UE_API virtual void AddCheats(bool bForce) override;
	UE_API virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;
	UE_API virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
	UE_API virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	UE_API virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface

	//~ILyraCameraAssistInterface interface
	UE_API virtual void OnCameraPenetratingTarget() override;
	//~End of ILyraCameraAssistInterface interface
	
	//~ILyraTeamAgentInterface interface
	UE_API virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	UE_API virtual FGenericTeamId GetGenericTeamId() const override;
	UE_API virtual FOnRPGTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of ILyraTeamAgentInterface interface

	UFUNCTION(BlueprintCallable, Category = "RPG|Character")
	UE_API void SetIsAutoRunning(const bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "RPG|Character")
	UE_API bool GetIsAutoRunning() const;

private:
	UPROPERTY()
	FOnRPGTeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY()
	TObjectPtr<APlayerState> LastSeenPlayerState;

private:
	UFUNCTION()
	void OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

protected:
	// Called when the player state is set or cleared
	UE_API virtual void OnPlayerStateChanged();

private:
	void BroadcastOnPlayerStateChanged();

protected:

	//~APlayerController interface

	//~End of APlayerController interface

	UE_API void OnSettingsChanged(URPGSettingsShared* Settings);
	
	UE_API void OnStartAutoRun();
	UE_API void OnEndAutoRun();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartAutoRun"))
	UE_API void K2_OnStartAutoRun();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndAutoRun"))
	UE_API void K2_OnEndAutoRun();

	bool bHideViewTargetPawnNextFrame = false;
};


// A player controller used for replay capture and playback
UCLASS()
class ARPGReplayPlayerController : public ARPGPlayerController
{
	GENERATED_BODY()

	virtual void Tick(float DeltaSeconds) override;
	virtual void SmoothTargetViewRotation(APawn* TargetPawn, float DeltaSeconds) override;
	virtual bool ShouldRecordClientReplay() override;

	// Callback for when the game state's RecorderPlayerState gets replicated during replay playback
	void RecorderPlayerStateUpdated(APlayerState* NewRecorderPlayerState);

	// Callback for when the followed player state changes pawn
	UFUNCTION()
	void OnPlayerStatePawnSet(APlayerState* ChangedPlayerState, APawn* NewPlayerPawn, APawn* OldPlayerPawn);

	// The player state we are currently following */
	UPROPERTY(Transient)
	TObjectPtr<APlayerState> FollowedPlayerState;
};

#undef UE_API
