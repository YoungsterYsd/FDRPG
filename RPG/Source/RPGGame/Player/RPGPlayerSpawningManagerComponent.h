// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/GameStateComponent.h"

#include "RPGPlayerSpawningManagerComponent.generated.h"

#define UE_API RPGGAME_API

class AController;
class APlayerController;
class APlayerState;
class APlayerStart;
class ARPGPlayerStart;
class AActor;

/**
 * @class ULyraPlayerSpawningManagerComponent
 */
UCLASS(MinimalAPI)
class URPGPlayerSpawningManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UE_API URPGPlayerSpawningManagerComponent(const FObjectInitializer& ObjectInitializer);

	/** UActorComponent */
	UE_API virtual void InitializeComponent() override;
	UE_API virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	/** ~UActorComponent */

protected:
	// Utility
	UE_API APlayerStart* GetFirstRandomUnoccupiedPlayerStart(AController* Controller, const TArray<ARPGPlayerStart*>& FoundStartPoints) const;
	
	virtual AActor* OnChoosePlayerStart(AController* Player, TArray<ARPGPlayerStart*>& PlayerStarts) { return nullptr; }
	virtual void OnFinishRestartPlayer(AController* Player, const FRotator& StartRotation) { }

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName=OnFinishRestartPlayer))
	UE_API void K2_OnFinishRestartPlayer(AController* Player, const FRotator& StartRotation);

private:

	/** We proxy these calls from ALyraGameMode, to this component so that each experience can more easily customize the respawn system they want. */
	UE_API AActor* ChoosePlayerStart(AController* Player);
	UE_API bool ControllerCanRestart(AController* Player);
	UE_API void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation);
	friend class ARPGGameMode;
	/** ~ALyraGameMode */

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<ARPGPlayerStart>> CachedPlayerStarts;

private:
	UE_API void OnLevelAdded(ULevel* InLevel, UWorld* InWorld);
	UE_API void HandleOnActorSpawned(AActor* SpawnedActor);

#if WITH_EDITOR
	UE_API APlayerStart* FindPlayFromHereStart(AController* Player);
#endif
};

#undef UE_API
