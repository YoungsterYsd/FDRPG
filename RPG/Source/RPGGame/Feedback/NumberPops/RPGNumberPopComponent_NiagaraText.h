// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RPGNumberPopComponent.h"

#include "RPGNumberPopComponent_NiagaraText.generated.h"

class URPGDamagePopStyleNiagara;
class UNiagaraComponent;
class UObject;

UCLASS(Blueprintable)
class URPGNumberPopComponent_NiagaraText : public URPGNumberPopComponent
{
	GENERATED_BODY()

public:

	URPGNumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ULyraNumberPopComponent interface
	virtual void AddNumberPop(const FRPGNumberPopRequest& NewRequest) override;
	//~End of ULyraNumberPopComponent interface

protected:
	
	TArray<int32> DamageNumberArray;

	/** Style patterns to attempt to apply to the incoming number pops */
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<URPGDamagePopStyleNiagara> Style;

	//Niagara Component used to display the damage
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UNiagaraComponent> NiagaraComp;
};
