// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ControllerComponent.h"
#include "RPGCharacterPartTypes.h"

#include "RPGControllerComponent_CharacterParts.generated.h"

class APawn;
class URPGPawnComponent_CharacterParts;
class UObject;
struct FFrame;

enum class ECharacterPartSource : uint8
{
	Natural,

	NaturalSuppressedViaCheat,

	AppliedViaDeveloperSettingsCheat,

	AppliedViaCheatManager
};

//////////////////////////////////////////////////////////////////////

// A character part requested on a controller component
USTRUCT()
struct FRPGControllerCharacterPartEntry
{
	GENERATED_BODY()

	FRPGControllerCharacterPartEntry()
	{}

public:
	// The character part being represented
	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties))
	FRPGCharacterPart Part;

	// The handle if already applied to a pawn
	FRPGCharacterPartHandle Handle;

	// The source of this part
	ECharacterPartSource Source = ECharacterPartSource::Natural;
};

//////////////////////////////////////////////////////////////////////

// A component that configure what cosmetic actors to spawn for the owning controller when it possesses a pawn
UCLASS(meta = (BlueprintSpawnableComponent))
class URPGControllerComponent_CharacterParts : public UControllerComponent
{
	GENERATED_BODY()

public:
	URPGControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	// Adds a character part to the actor that owns this customization component, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void AddCharacterPart(const FRPGCharacterPart& NewPart);

	// Removes a previously added character part from the actor that owns this customization component, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void RemoveCharacterPart(const FRPGCharacterPart& PartToRemove);

	// Removes all added character parts, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void RemoveAllCharacterParts();

	// Applies relevant developer settings if in PIE
	void ApplyDeveloperSettings();

protected:
	UPROPERTY(EditAnywhere, Category=Cosmetics)
	TArray<FRPGControllerCharacterPartEntry> CharacterParts;

private:
	URPGPawnComponent_CharacterParts* GetPawnCustomizer() const;

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	void AddCharacterPartInternal(const FRPGCharacterPart& NewPart, ECharacterPartSource Source);

	void AddCheatPart(const FRPGCharacterPart& NewPart, bool bSuppressNaturalParts);
	void ClearCheatParts();

	void SetSuppressionOnNaturalParts(bool bSuppressed);

	friend class URPGCosmeticCheats;
};
