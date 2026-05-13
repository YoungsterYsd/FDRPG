// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/GameEngine.h"

#include "RPGGameEngine.generated.h"

class IEngineLoop;
class UObject;


UCLASS()
class URPGGameEngine : public UGameEngine
{
	GENERATED_BODY()

public:

	URPGGameEngine(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void Init(IEngineLoop* InEngineLoop) override;
};
