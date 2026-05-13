// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RPGTeamInfoBase.h"

#include "RPGTeamPrivateInfo.generated.h"

class UObject;

UCLASS()
class ARPGTeamPrivateInfo : public ARPGTeamInfoBase
{
	GENERATED_BODY()

public:
	ARPGTeamPrivateInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
