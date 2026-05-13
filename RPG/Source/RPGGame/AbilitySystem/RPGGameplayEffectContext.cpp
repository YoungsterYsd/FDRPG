// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGGameplayEffectContext.h"

#include "AbilitySystem/RPGAbilitySourceInterface.h"
#include "Engine/HitResult.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"
#include "Serialization/GameplayEffectContextNetSerializer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGGameplayEffectContext)

class FArchive;

FRPGGameplayEffectContext* FRPGGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FRPGGameplayEffectContext::StaticStruct()))
	{
		return (FRPGGameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

bool FRPGGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// CartridgeID

	return true;
}

namespace UE::Net
{
	// Forward to FGameplayEffectContextNetSerializer
	// Note: If FLyraGameplayEffectContext::NetSerialize() is modified, a custom NetSerializer must be implemented as the current fallback will no longer be sufficient.
	UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES(RPGGameplayEffectContext, FGameplayEffectContextNetSerializer);
}

void FRPGGameplayEffectContext::SetAbilitySource(const IRPGAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

const IRPGAbilitySourceInterface* FRPGGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IRPGAbilitySourceInterface>(AbilitySourceObject.Get());
}

const UPhysicalMaterial* FRPGGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}

