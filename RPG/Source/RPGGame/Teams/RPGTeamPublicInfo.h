// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RPGTeamInfoBase.h"

#include "RPGTeamPublicInfo.generated.h"

class URPGTeamCreationComponent;
class URPGTeamDisplayAsset;
class UObject;
struct FFrame;

UCLASS()
class ARPGTeamPublicInfo : public ARPGTeamInfoBase
{
	GENERATED_BODY()

	friend URPGTeamCreationComponent;

public:
	ARPGTeamPublicInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	URPGTeamDisplayAsset* GetTeamDisplayAsset() const { return TeamDisplayAsset; }

private:
	UFUNCTION()
	void OnRep_TeamDisplayAsset();

	void SetTeamDisplayAsset(TObjectPtr<URPGTeamDisplayAsset> NewDisplayAsset);

private:
	UPROPERTY(ReplicatedUsing=OnRep_TeamDisplayAsset)
	TObjectPtr<URPGTeamDisplayAsset> TeamDisplayAsset;
};
