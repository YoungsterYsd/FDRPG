// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGUICameraManagerComponent.h"

#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "RPGPlayerCameraManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGUICameraManagerComponent)

class AActor;
class FDebugDisplayInfo;

URPGUICameraManagerComponent* URPGUICameraManagerComponent::GetComponent(APlayerController* PC)
{
	if (PC != nullptr)
	{
		if (ARPGPlayerCameraManager* PCCamera = Cast<ARPGPlayerCameraManager>(PC->PlayerCameraManager))
		{
			return PCCamera->GetUICameraComponent();
		}
	}

	return nullptr;
}

URPGUICameraManagerComponent::URPGUICameraManagerComponent()
{
	bWantsInitializeComponent = true;

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		// Register "showdebug" hook.
		if (!IsRunningDedicatedServer())
		{
			AHUD::OnShowDebugInfo.AddUObject(this, &ThisClass::OnShowDebugInfo);
		}
	}
}

void URPGUICameraManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void URPGUICameraManagerComponent::SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams)
{
	TGuardValue<bool> UpdatingViewTargetGuard(bUpdatingViewTarget, true);

	ViewTarget = InViewTarget;
	CastChecked<ARPGPlayerCameraManager>(GetOwner())->SetViewTarget(ViewTarget, TransitionParams);
}

bool URPGUICameraManagerComponent::NeedsToUpdateViewTarget() const
{
	return false;
}

void URPGUICameraManagerComponent::UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime)
{
}

void URPGUICameraManagerComponent::OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos)
{
}
