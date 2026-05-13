// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonGameViewportClient.h"

#include "RPGGameViewportClient.generated.h"

class UGameInstance;
class UObject;

UCLASS(BlueprintType)
class URPGGameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()

public:
	URPGGameViewportClient();

	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true) override;
};
