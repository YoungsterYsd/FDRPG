// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGGameplayAbility_FromEquipment.h"
#include "RPGEquipmentInstance.h"
#include "Inventory/RPGInventoryItemInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGGameplayAbility_FromEquipment)

URPGGameplayAbility_FromEquipment::URPGGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

URPGEquipmentInstance* URPGGameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<URPGEquipmentInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}

URPGInventoryItemInstance* URPGGameplayAbility_FromEquipment::GetAssociatedItem() const
{
	if (URPGEquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		return Cast<URPGInventoryItemInstance>(Equipment->GetInstigator());
	}
	return nullptr;
}


#if WITH_EDITOR
EDataValidationResult URPGGameplayAbility_FromEquipment::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

PRAGMA_DISABLE_DEPRECATION_WARNINGS
	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
PRAGMA_ENABLE_DEPRECATION_WARNINGS
	{
		Context.AddError(NSLOCTEXT("Lyra", "EquipmentAbilityMustBeInstanced", "Equipment ability must be instanced"));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif
