// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "RPGInventoryManagerComponent.generated.h"

#define UE_API RPGGAME_API

class URPGInventoryItemDefinition;
class URPGInventoryItemInstance;
class URPGInventoryManagerComponent;
class UObject;
struct FFrame;
struct FRPGInventoryList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A message when an item is added to the inventory */
USTRUCT(BlueprintType)
struct FRPGInventoryChangeMessage
{
	GENERATED_BODY()

	//@TODO: Tag based names+owning actors for inventories instead of directly exposing the component?
	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<URPGInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 Delta = 0;
};

/** A single entry in an inventory */
USTRUCT(BlueprintType)
struct FRPGInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FRPGInventoryEntry()
	{}

	FString GetDebugString() const;

private:
	friend FRPGInventoryList;
	friend URPGInventoryManagerComponent;

	UPROPERTY()
	TObjectPtr<URPGInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
};

/** List of inventory items */
USTRUCT(BlueprintType)
struct FRPGInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FRPGInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FRPGInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	TArray<URPGInventoryItemInstance*> GetAllItems() const;

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FRPGInventoryEntry, FRPGInventoryList>(Entries, DeltaParms, *this);
	}

	URPGInventoryItemInstance* AddEntry(TSubclassOf<URPGInventoryItemDefinition> ItemClass, int32 StackCount);
	void AddEntry(URPGInventoryItemInstance* Instance);

	void RemoveEntry(URPGInventoryItemInstance* Instance);

private:
	void BroadcastChangeMessage(FRPGInventoryEntry& Entry, int32 OldCount, int32 NewCount);

private:
	friend URPGInventoryManagerComponent;

private:
	// Replicated list of items
	UPROPERTY()
	TArray<FRPGInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FRPGInventoryList> : public TStructOpsTypeTraitsBase2<FRPGInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};










/**
 * Manages an inventory
 */
UCLASS(MinimalAPI, BlueprintType)
class URPGInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UE_API URPGInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	UE_API bool CanAddItemDefinition(TSubclassOf<URPGInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	UE_API URPGInventoryItemInstance* AddItemDefinition(TSubclassOf<URPGInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	UE_API void AddItemInstance(URPGInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	UE_API void RemoveItemInstance(URPGInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure=false)
	UE_API TArray<URPGInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure)
	UE_API URPGInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<URPGInventoryItemDefinition> ItemDef) const;

	UE_API int32 GetTotalItemCountByDefinition(TSubclassOf<URPGInventoryItemDefinition> ItemDef) const;
	UE_API bool ConsumeItemsByDefinition(TSubclassOf<URPGInventoryItemDefinition> ItemDef, int32 NumToConsume);

	//~UObject interface
	UE_API virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	UE_API virtual void ReadyForReplication() override;
	//~End of UObject interface

private:
	UPROPERTY(Replicated)
	FRPGInventoryList InventoryList;
};

#undef UE_API
