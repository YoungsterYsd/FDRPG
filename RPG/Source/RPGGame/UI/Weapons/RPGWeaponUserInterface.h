// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"

#include "RPGWeaponUserInterface.generated.h"

class URPGWeaponInstance;
class UObject;
struct FGeometry;

UCLASS()
class URPGWeaponUserInterface : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	URPGWeaponUserInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponChanged(URPGWeaponInstance* OldWeapon, URPGWeaponInstance* NewWeapon);

private:
	void RebuildWidgetFromWeapon();

private:
	UPROPERTY(Transient)
	TObjectPtr<URPGWeaponInstance> CurrentInstance;
};
