// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGTeamPublicInfo.h"

#include "Net/UnrealNetwork.h"
#include "Teams/RPGTeamInfoBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGTeamPublicInfo)

class FLifetimeProperty;

ARPGTeamPublicInfo::ARPGTeamPublicInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ARPGTeamPublicInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TeamDisplayAsset, COND_InitialOnly);
}

void ARPGTeamPublicInfo::SetTeamDisplayAsset(TObjectPtr<URPGTeamDisplayAsset> NewDisplayAsset)
{
	check(HasAuthority());
	check(TeamDisplayAsset == nullptr);

	TeamDisplayAsset = NewDisplayAsset;

	TryRegisterWithTeamSubsystem();
}

void ARPGTeamPublicInfo::OnRep_TeamDisplayAsset()
{
	TryRegisterWithTeamSubsystem();
}

