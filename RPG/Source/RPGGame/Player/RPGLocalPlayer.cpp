// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/RPGLocalPlayer.h"

#include "AudioMixerBlueprintLibrary.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Settings/RPGSettingsLocal.h"
#include "Settings/RPGSettingsShared.h"
#include "CommonUserSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGLocalPlayer)

class UObject;

URPGLocalPlayer::URPGLocalPlayer()
{
}

void URPGLocalPlayer::PostInitProperties()
{
	Super::PostInitProperties();

	if (URPGSettingsLocal* LocalSettings = GetLocalSettings())
	{
		LocalSettings->OnAudioOutputDeviceChanged.AddUObject(this, &URPGLocalPlayer::OnAudioOutputDeviceChanged);
	}
}

void URPGLocalPlayer::SwitchController(class APlayerController* PC)
{
	Super::SwitchController(PC);

	OnPlayerControllerChanged(PlayerController);
}

bool URPGLocalPlayer::SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld)
{
	const bool bResult = Super::SpawnPlayActor(URL, OutError, InWorld);

	OnPlayerControllerChanged(PlayerController);

	return bResult;
}

void URPGLocalPlayer::InitOnlineSession()
{
	OnPlayerControllerChanged(PlayerController);

	Super::InitOnlineSession();
}

void URPGLocalPlayer::OnPlayerControllerChanged(APlayerController* NewController)
{
	// Stop listening for changes from the old controller
	FGenericTeamId OldTeamID = FGenericTeamId::NoTeam;
	if (IRPGTeamAgentInterface* ControllerAsTeamProvider = Cast<IRPGTeamAgentInterface>(LastBoundPC.Get()))
	{
		OldTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().RemoveAll(this);
	}

	// Grab the current team ID and listen for future changes
	FGenericTeamId NewTeamID = FGenericTeamId::NoTeam;
	if (IRPGTeamAgentInterface* ControllerAsTeamProvider = Cast<IRPGTeamAgentInterface>(NewController))
	{
		NewTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);
		LastBoundPC = NewController;
	}

	ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);
}

void URPGLocalPlayer::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	// Do nothing, we merely observe the team of our associated player controller
}

FGenericTeamId URPGLocalPlayer::GetGenericTeamId() const
{
	if (IRPGTeamAgentInterface* ControllerAsTeamProvider = Cast<IRPGTeamAgentInterface>(PlayerController))
	{
		return ControllerAsTeamProvider->GetGenericTeamId();
	}
	else
	{
		return FGenericTeamId::NoTeam;
	}
}

FOnRPGTeamIndexChangedDelegate* URPGLocalPlayer::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

URPGSettingsLocal* URPGLocalPlayer::GetLocalSettings() const
{
	return URPGSettingsLocal::Get();
}

URPGSettingsShared* URPGLocalPlayer::GetSharedSettings() const
{
	if (!SharedSettings)
	{
		// On PC it's okay to use the sync load because it only checks the disk
		// This could use a platform tag to check for proper save support instead
		bool bCanLoadBeforeLogin = PLATFORM_DESKTOP;
		
		if (bCanLoadBeforeLogin)
		{
			SharedSettings = URPGSettingsShared::LoadOrCreateSettings(this);
		}
		else
		{
			// We need to wait for user login to get the real settings so return temp ones
			SharedSettings = URPGSettingsShared::CreateTemporarySettings(this);
		}
	}

	return SharedSettings;
}

void URPGLocalPlayer::LoadSharedSettingsFromDisk(bool bForceLoad)
{
	FUniqueNetIdRepl CurrentNetId = GetCachedUniqueNetId();
	if (!bForceLoad && SharedSettings && CurrentNetId == NetIdForSharedSettings)
	{
		// Already loaded once, don't reload
		return;
	}

	ensure(URPGSettingsShared::AsyncLoadOrCreateSettings(this, URPGSettingsShared::FOnSettingsLoadedEvent::CreateUObject(this, &URPGLocalPlayer::OnSharedSettingsLoaded)));
}

void URPGLocalPlayer::OnSharedSettingsLoaded(URPGSettingsShared* LoadedOrCreatedSettings)
{
	// The settings are applied before it gets here
	if (ensure(LoadedOrCreatedSettings))
	{
		// This will replace the temporary or previously loaded object which will GC out normally
		SharedSettings = LoadedOrCreatedSettings;

		NetIdForSharedSettings = GetCachedUniqueNetId();
	}
}

void URPGLocalPlayer::OnAudioOutputDeviceChanged(const FString& InAudioOutputDeviceId)
{
	FOnCompletedDeviceSwap DevicesSwappedCallback;
	DevicesSwappedCallback.BindUFunction(this, FName("OnCompletedAudioDeviceSwap"));
	UAudioMixerBlueprintLibrary::SwapAudioOutputDevice(GetWorld(), InAudioOutputDeviceId, DevicesSwappedCallback);
}

void URPGLocalPlayer::OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult)
{
	if (SwapResult.Result == ESwapAudioOutputDeviceResultState::Failure)
	{
	}
}

void URPGLocalPlayer::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	ConditionalBroadcastTeamChanged(this, IntegerToGenericTeamId(OldTeam), IntegerToGenericTeamId(NewTeam));
}

