// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGVerbMessageReplication.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "Messages/RPGVerbMessage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGVerbMessageReplication)

//////////////////////////////////////////////////////////////////////
// FLyraVerbMessageReplicationEntry

FString FRPGVerbMessageReplicationEntry::GetDebugString() const
{
	return Message.ToString();
}

//////////////////////////////////////////////////////////////////////
// FLyraVerbMessageReplication

void FRPGVerbMessageReplication::AddMessage(const FRPGVerbMessage& Message)
{
	FRPGVerbMessageReplicationEntry& NewStack = CurrentMessages.Emplace_GetRef(Message);
	MarkItemDirty(NewStack);
}

void FRPGVerbMessageReplication::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
// 	for (int32 Index : RemovedIndices)
// 	{
// 		const FGameplayTag Tag = CurrentMessages[Index].Tag;
// 		TagToCountMap.Remove(Tag);
// 	}
}

void FRPGVerbMessageReplication::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FRPGVerbMessageReplicationEntry& Entry = CurrentMessages[Index];
		RebroadcastMessage(Entry.Message);
	}
}

void FRPGVerbMessageReplication::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FRPGVerbMessageReplicationEntry& Entry = CurrentMessages[Index];
		RebroadcastMessage(Entry.Message);
	}
}

void FRPGVerbMessageReplication::RebroadcastMessage(const FRPGVerbMessage& Message)
{
	check(Owner);
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(Owner);
	MessageSystem.BroadcastMessage(Message.Verb, Message);
}

