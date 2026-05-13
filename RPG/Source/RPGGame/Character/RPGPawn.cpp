// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGPawn.h"

#include "GameFramework/Controller.h"
#include "RPGLogChannels.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ScriptInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGPawn)

class FLifetimeProperty;
class UObject;

ARPGPawn::ARPGPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ARPGPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MyTeamID);
}

void ARPGPawn::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ARPGPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ARPGPawn::PossessedBy(AController* NewController)
{
	const FGenericTeamId OldTeamID = MyTeamID;

	Super::PossessedBy(NewController);

	// Grab the current team ID and listen for future changes
	if (IRPGTeamAgentInterface* ControllerAsTeamProvider = Cast<IRPGTeamAgentInterface>(NewController))
	{
		MyTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);
	}
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void ARPGPawn::UnPossessed()
{
	AController* const OldController = GetController();

	// Stop listening for changes from the old controller
	const FGenericTeamId OldTeamID = MyTeamID;
	if (IRPGTeamAgentInterface* ControllerAsTeamProvider = Cast<IRPGTeamAgentInterface>(OldController))
	{
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().RemoveAll(this);
	}

	Super::UnPossessed();

	// Determine what the new team ID should be afterwards
	MyTeamID = DetermineNewTeamAfterPossessionEnds(OldTeamID);
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void ARPGPawn::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (GetController() == nullptr)
	{
		if (HasAuthority())
		{
			const FGenericTeamId OldTeamID = MyTeamID;
			MyTeamID = NewTeamID;
			ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
		}
		else
		{
			UE_LOG(LogRPGTeams, Error, TEXT("You can't set the team ID on a pawn (%s) except on the authority"), *GetPathNameSafe(this));
		}
	}
	else
	{
		UE_LOG(LogRPGTeams, Error, TEXT("You can't set the team ID on a possessed pawn (%s); it's driven by the associated controller"), *GetPathNameSafe(this));
	}
}

FGenericTeamId ARPGPawn::GetGenericTeamId() const
{
	return MyTeamID;
}

FOnRPGTeamIndexChangedDelegate* ARPGPawn::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void ARPGPawn::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	const FGenericTeamId MyOldTeamID = MyTeamID;
	MyTeamID = IntegerToGenericTeamId(NewTeam);
	ConditionalBroadcastTeamChanged(this, MyOldTeamID, MyTeamID);
}

void ARPGPawn::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

