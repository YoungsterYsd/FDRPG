// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "RPGVerbMessage.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "RPGVerbMessageReplication.generated.h"

class UObject;
struct FRPGVerbMessageReplication;
struct FNetDeltaSerializeInfo;

/**
 * Represents one verb message
 */
USTRUCT(BlueprintType)
struct FRPGVerbMessageReplicationEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FRPGVerbMessageReplicationEntry()
	{}

	FRPGVerbMessageReplicationEntry(const FRPGVerbMessage& InMessage)
		: Message(InMessage)
	{
	}

	FString GetDebugString() const;

private:
	friend FRPGVerbMessageReplication;

	UPROPERTY()
	FRPGVerbMessage Message;
};

/** Container of verb messages to replicate */
USTRUCT(BlueprintType)
struct FRPGVerbMessageReplication : public FFastArraySerializer
{
	GENERATED_BODY()

	FRPGVerbMessageReplication()
	{
	}

public:
	void SetOwner(UObject* InOwner) { Owner = InOwner; }

	// Broadcasts a message from server to clients
	void AddMessage(const FRPGVerbMessage& Message);

	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FRPGVerbMessageReplicationEntry, FRPGVerbMessageReplication>(CurrentMessages, DeltaParms, *this);
	}

private:
	void RebroadcastMessage(const FRPGVerbMessage& Message);

private:
	// Replicated list of gameplay tag stacks
	UPROPERTY()
	TArray<FRPGVerbMessageReplicationEntry> CurrentMessages;
	
	// Owner (for a route to a world)
	UPROPERTY()
	TObjectPtr<UObject> Owner = nullptr;
};

template<>
struct TStructOpsTypeTraits<FRPGVerbMessageReplication> : public TStructOpsTypeTraitsBase2<FRPGVerbMessageReplication>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
