// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGWeaponUserInterface.h"

#include "Equipment/RPGEquipmentManagerComponent.h"
#include "GameFramework/Pawn.h"
#include "Weapons/RPGWeaponInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGWeaponUserInterface)

struct FGeometry;

URPGWeaponUserInterface::URPGWeaponUserInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URPGWeaponUserInterface::NativeConstruct()
{
	Super::NativeConstruct();
}

void URPGWeaponUserInterface::NativeDestruct()
{
	Super::NativeDestruct();
}

void URPGWeaponUserInterface::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		if (URPGEquipmentManagerComponent* EquipmentManager = Pawn->FindComponentByClass<URPGEquipmentManagerComponent>())
		{
			if (URPGWeaponInstance* NewInstance = EquipmentManager->GetFirstInstanceOfType<URPGWeaponInstance>())
			{
				if (NewInstance != CurrentInstance && NewInstance->GetInstigator() != nullptr)
				{
					URPGWeaponInstance* OldWeapon = CurrentInstance;
					CurrentInstance = NewInstance;
					RebuildWidgetFromWeapon();
					OnWeaponChanged(OldWeapon, CurrentInstance);
				}
			}
		}
	}
}

void URPGWeaponUserInterface::RebuildWidgetFromWeapon()
{
	
}

