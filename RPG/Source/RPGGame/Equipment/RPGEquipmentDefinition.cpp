// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGEquipmentDefinition.h"
#include "RPGEquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGEquipmentDefinition)

URPGEquipmentDefinition::URPGEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstanceType = URPGEquipmentInstance::StaticClass();
}

