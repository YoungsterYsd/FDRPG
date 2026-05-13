// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGCosmeticCheats.h"
#include "Cosmetics/RPGCharacterPartTypes.h"
#include "RPGControllerComponent_CharacterParts.h"
#include "GameFramework/CheatManagerDefines.h"
#include "System/RPGDevelopmentStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGCosmeticCheats)

//////////////////////////////////////////////////////////////////////
// ULyraCosmeticCheats

URPGCosmeticCheats::URPGCosmeticCheats()
{
#if UE_WITH_CHEAT_MANAGER
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		UCheatManager::RegisterForOnCheatManagerCreated(FOnCheatManagerCreated::FDelegate::CreateLambda(
			[](UCheatManager* CheatManager)
			{
				CheatManager->AddCheatManagerExtension(NewObject<ThisClass>(CheatManager));
			}));
	}
#endif
}

void URPGCosmeticCheats::AddCharacterPart(const FString& AssetName, bool bSuppressNaturalParts)
{
#if UE_WITH_CHEAT_MANAGER
	if (URPGControllerComponent_CharacterParts* CosmeticComponent = GetCosmeticComponent())
	{
		TSubclassOf<AActor> PartClass = URPGDevelopmentStatics::FindClassByShortName<AActor>(AssetName);
		if (PartClass != nullptr)
		{
			FRPGCharacterPart Part;
			Part.PartClass = PartClass;

			CosmeticComponent->AddCheatPart(Part, bSuppressNaturalParts);
		}
	}
#endif	
}

void URPGCosmeticCheats::ReplaceCharacterPart(const FString& AssetName, bool bSuppressNaturalParts)
{
	ClearCharacterPartOverrides();
	AddCharacterPart(AssetName, bSuppressNaturalParts);
}

void URPGCosmeticCheats::ClearCharacterPartOverrides()
{
#if UE_WITH_CHEAT_MANAGER
	if (URPGControllerComponent_CharacterParts* CosmeticComponent = GetCosmeticComponent())
	{
		CosmeticComponent->ClearCheatParts();
	}
#endif	
}

URPGControllerComponent_CharacterParts* URPGCosmeticCheats::GetCosmeticComponent() const
{
	if (APlayerController* PC = GetPlayerController())
	{
		return PC->FindComponentByClass<URPGControllerComponent_CharacterParts>();
	}

	return nullptr;
}

