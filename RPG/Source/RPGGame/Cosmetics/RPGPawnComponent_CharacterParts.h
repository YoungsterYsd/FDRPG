// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/PawnComponent.h"
#include "Cosmetics/RPGCosmeticAnimationTypes.h"
#include "RPGCharacterPartTypes.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "RPGPawnComponent_CharacterParts.generated.h"

class URPGPawnComponent_CharacterParts;
namespace EEndPlayReason { enum Type : int; }
struct FGameplayTag;
struct FRPGCharacterPartList;

class AActor;
class UChildActorComponent;
class UObject;
class USceneComponent;
class USkeletalMeshComponent;
struct FFrame;
struct FNetDeltaSerializeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGSpawnedCharacterPartsChanged, URPGPawnComponent_CharacterParts*, ComponentWithChangedParts);

//////////////////////////////////////////////////////////////////////

// A single applied character part
USTRUCT()
struct FRPGAppliedCharacterPartEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FRPGAppliedCharacterPartEntry()
	{}

	FString GetDebugString() const;

private:
	friend FRPGCharacterPartList;
	friend URPGPawnComponent_CharacterParts;

private:
	// The character part being represented
	UPROPERTY()
	FRPGCharacterPart Part;

	// Handle index we returned to the user (server only)
	UPROPERTY(NotReplicated)
	int32 PartHandle = INDEX_NONE;

	// The spawned actor instance (client only)
	UPROPERTY(NotReplicated)
	TObjectPtr<UChildActorComponent> SpawnedComponent = nullptr;
};

//////////////////////////////////////////////////////////////////////

// Replicated list of applied character parts
USTRUCT(BlueprintType)
struct FRPGCharacterPartList : public FFastArraySerializer
{
	GENERATED_BODY()

	FRPGCharacterPartList()
		: OwnerComponent(nullptr)
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
		return FFastArraySerializer::FastArrayDeltaSerialize<FRPGAppliedCharacterPartEntry, FRPGCharacterPartList>(Entries, DeltaParms, *this);
	}

	FRPGCharacterPartHandle AddEntry(FRPGCharacterPart NewPart);
	void RemoveEntry(FRPGCharacterPartHandle Handle);
	void ClearAllEntries(bool bBroadcastChangeDelegate);

	FGameplayTagContainer CollectCombinedTags() const;

	void SetOwnerComponent(URPGPawnComponent_CharacterParts* InOwnerComponent)
	{
		OwnerComponent = InOwnerComponent;
	}
	
private:
	friend URPGPawnComponent_CharacterParts;

	bool SpawnActorForEntry(FRPGAppliedCharacterPartEntry& Entry);
	bool DestroyActorForEntry(FRPGAppliedCharacterPartEntry& Entry);

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FRPGAppliedCharacterPartEntry> Entries;

	// The component that contains this list
	UPROPERTY(NotReplicated)
	TObjectPtr<URPGPawnComponent_CharacterParts> OwnerComponent;

	// Upcounter for handles
	int32 PartHandleCounter = 0;
};

template<>
struct TStructOpsTypeTraits<FRPGCharacterPartList> : public TStructOpsTypeTraitsBase2<FRPGCharacterPartList>
{
	enum { WithNetDeltaSerializer = true };
};

//////////////////////////////////////////////////////////////////////

// A component that handles spawning cosmetic actors attached to the owner pawn on all clients
UCLASS(meta=(BlueprintSpawnableComponent))
class URPGPawnComponent_CharacterParts : public UPawnComponent
{
	GENERATED_BODY()

public:
	URPGPawnComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnRegister() override;
	//~End of UActorComponent interface

	// Adds a character part to the actor that owns this customization component, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	FRPGCharacterPartHandle AddCharacterPart(const FRPGCharacterPart& NewPart);

	// Removes a previously added character part from the actor that owns this customization component, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void RemoveCharacterPart(FRPGCharacterPartHandle Handle);

	// Removes all added character parts, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void RemoveAllCharacterParts();

	// Gets the list of all spawned character parts from this component
	UFUNCTION(BlueprintCallable, BlueprintPure=false, BlueprintCosmetic, Category=Cosmetics)
	TArray<AActor*> GetCharacterPartActors() const;

	// If the parent actor is derived from ACharacter, returns the Mesh component, otherwise nullptr
	USkeletalMeshComponent* GetParentMeshComponent() const;

	// Returns the scene component to attach the spawned actors to
	// If the parent actor is derived from ACharacter, we'll use the Mesh component, otherwise the root component
	USceneComponent* GetSceneComponentToAttachTo() const;

	// Returns the set of combined gameplay tags from attached character parts, optionally filtered to only tags that start with the specified root
	UFUNCTION(BlueprintCallable, BlueprintPure=false, BlueprintCosmetic, Category=Cosmetics)
	FGameplayTagContainer GetCombinedTags(FGameplayTag RequiredPrefix) const;

	void BroadcastChanged();

public:
	// Delegate that will be called when the list of spawned character parts has changed
	UPROPERTY(BlueprintAssignable, Category=Cosmetics, BlueprintCallable)
	FRPGSpawnedCharacterPartsChanged OnCharacterPartsChanged;

private:
	// List of character parts
	UPROPERTY(Replicated, Transient)
	FRPGCharacterPartList CharacterPartList;

	// Rules for how to pick a body style mesh for animation to play on, based on character part cosmetics tags
	UPROPERTY(EditAnywhere, Category=Cosmetics)
	FRPGAnimBodyStyleSelectionSet BodyMeshes;
};
