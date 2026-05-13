// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/RPGAbilitySet.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "RPGEquipmentManagerComponent.generated.h"

#define UE_API RPGGAME_API

class UActorComponent;
class URPGAbilitySystemComponent;
class URPGEquipmentDefinition;
class URPGEquipmentInstance;
class URPGEquipmentManagerComponent;
class UObject;
struct FFrame;
struct FRPGEquipmentList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A single piece of applied equipment */
USTRUCT(BlueprintType)
struct FRPGAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FRPGAppliedEquipmentEntry()
	{}

	FString GetDebugString() const;

private:
	friend FRPGEquipmentList;
	friend URPGEquipmentManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<URPGEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<URPGEquipmentInstance> Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	FRPGAbilitySet_GrantedHandles GrantedHandles;
};

/** List of applied equipment */
USTRUCT(BlueprintType)
struct FRPGEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FRPGEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FRPGEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FRPGAppliedEquipmentEntry, FRPGEquipmentList>(Entries, DeltaParms, *this);
	}

	URPGEquipmentInstance* AddEntry(TSubclassOf<URPGEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(URPGEquipmentInstance* Instance);

private:
	URPGAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend URPGEquipmentManagerComponent;

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FRPGAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FRPGEquipmentList> : public TStructOpsTypeTraitsBase2<FRPGEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};










/**
 * Manages equipment applied to a pawn
 */
UCLASS(MinimalAPI, BlueprintType, Const)
class URPGEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UE_API URPGEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UE_API URPGEquipmentInstance* EquipItem(TSubclassOf<URPGEquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UE_API void UnequipItem(URPGEquipmentInstance* ItemInstance);

	//~UObject interface
	UE_API virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	//virtual void EndPlay() override;
	UE_API virtual void InitializeComponent() override;
	UE_API virtual void UninitializeComponent() override;
	UE_API virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UE_API URPGEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<URPGEquipmentInstance> InstanceType);

 	/** Returns all equipped instances of a given type, or an empty array if none are found */
 	UFUNCTION(BlueprintCallable, BlueprintPure)
	UE_API TArray<URPGEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<URPGEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

private:
	UPROPERTY(Replicated)
	FRPGEquipmentList EquipmentList;
};

#undef UE_API
