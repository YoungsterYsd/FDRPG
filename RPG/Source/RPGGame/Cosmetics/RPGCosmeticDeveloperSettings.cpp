// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGCosmeticDeveloperSettings.h"
#include "Cosmetics/RPGCharacterPartTypes.h"
#include "Misc/App.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#include "System/RPGDevelopmentStatics.h"
#include "TimerManager.h"
#include "Engine/Engine.h"
#include "RPGControllerComponent_CharacterParts.h"
#include "EngineUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGCosmeticDeveloperSettings)

#define LOCTEXT_NAMESPACE "LyraCheats"

URPGCosmeticDeveloperSettings::URPGCosmeticDeveloperSettings()
{
}

FName URPGCosmeticDeveloperSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

#if WITH_EDITOR

void URPGCosmeticDeveloperSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	ApplySettings();
}

void URPGCosmeticDeveloperSettings::PostReloadConfig(FProperty* PropertyThatWasLoaded)
{
	Super::PostReloadConfig(PropertyThatWasLoaded);

	ApplySettings();
}

void URPGCosmeticDeveloperSettings::PostInitProperties()
{
	Super::PostInitProperties();

	ApplySettings();
}

void URPGCosmeticDeveloperSettings::ApplySettings()
{
	if (GIsEditor && (GEngine != nullptr))
	{
		ReapplyLoadoutIfInPIE();
	}
}

void URPGCosmeticDeveloperSettings::ReapplyLoadoutIfInPIE()
{
#if WITH_SERVER_CODE
	// Update the loadout on all players
	UWorld* ServerWorld = URPGDevelopmentStatics::FindPlayInEditorAuthorityWorld();
	if (ServerWorld != nullptr)
	{
		ServerWorld->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([=]()
			{
				for (TActorIterator<APlayerController> PCIterator(ServerWorld); PCIterator; ++PCIterator)
				{
					if (APlayerController* PC = *PCIterator)
					{
						if (URPGControllerComponent_CharacterParts* CosmeticComponent = PC->FindComponentByClass<URPGControllerComponent_CharacterParts>())
						{
							CosmeticComponent->ApplyDeveloperSettings();
						}
					}
				}
			}));
	}
#endif	// WITH_SERVER_CODE
}

void URPGCosmeticDeveloperSettings::OnPlayInEditorStarted() const
{
	// Show a notification toast to remind the user that there's an experience override set
	if (CheatCosmeticCharacterParts.Num() > 0)
	{
		FNotificationInfo Info(LOCTEXT("CosmeticOverrideActive", "Applying Cosmetic Override"));
		Info.ExpireDuration = 2.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
	}
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE

