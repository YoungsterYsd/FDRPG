// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGCameraModeStack.h"
#include "Camera/RPGCameraComponent.h"
#include "System/RPGLogChannels.h"
#include "Engine/Canvas.h"
#include "GameFramework/Actor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGCameraModeStack)

URPGCameraModeStack::URPGCameraModeStack()
{
}

URPGCameraMode* URPGCameraModeStack::PushCameraMode(TSubclassOf<URPGCameraMode> CameraModeClass)
{
	if (!CameraModeClass)
	{
		UE_LOG(LogRPG, Warning, TEXT("URPGCameraModeStack::PushCameraMode - Null CameraModeClass"));
		return nullptr;
	}

	// Check if this camera mode is already on top of the stack
	if (CameraModeStack.Num() > 0)
	{
		const FRPGCameraModeStackEntry& TopEntry = CameraModeStack.Last();
		if (TopEntry.CameraMode && TopEntry.CameraMode->GetClass() == CameraModeClass)
		{
			// Already on top, just reset the blend
			TopEntry.CameraMode->OnActivate();
			return TopEntry.CameraMode;
		}
	}

	// Create a new camera mode instance
	URPGCameraMode* NewCameraMode = CreateCameraMode(CameraModeClass);
	if (!NewCameraMode)
	{
		return nullptr;
	}

	// Deactivate the current top camera mode
	if (CameraModeStack.Num() > 0)
	{
		if (URPGCameraMode* TopMode = CameraModeStack.Last().CameraMode)
		{
			TopMode->OnDeactivate();
		}
	}

	// Create the stack entry
	FRPGCameraModeStackEntry NewEntry;
	NewEntry.CameraMode = NewCameraMode;
	NewEntry.BlendTimeRemaining = NewCameraMode->BlendTime;
	NewEntry.TotalBlendTime = NewCameraMode->BlendTime;

	// Push onto the stack
	CameraModeStack.Add(NewEntry);

	// Activate the new camera mode
	NewCameraMode->OnActivate();

	return NewCameraMode;
}

void URPGCameraModeStack::UpdateStack(float DeltaTime, FVector& OutCameraLocation, FRotator& OutCameraRotation, float& OutFOV)
{
	if (CameraModeStack.Num() == 0)
	{
		return;
	}

	// Update all camera modes and blend times
	for (int32 Index = CameraModeStack.Num() - 1; Index >= 0; --Index)
	{
		FRPGCameraModeStackEntry& Entry = CameraModeStack[Index];

		if (Entry.CameraMode)
		{
			Entry.CameraMode->UpdateCameraMode(DeltaTime);

			// Update blend time
			if (Entry.BlendTimeRemaining > 0.0f)
			{
				Entry.BlendTimeRemaining = FMath::Max(0.0f, Entry.BlendTimeRemaining - DeltaTime);
			}
		}
	}

	// Remove any fully blended-out modes from the bottom of the stack
	// (Keep at least one mode)
	while (CameraModeStack.Num() > 1)
	{
		FRPGCameraModeStackEntry& BottomEntry = CameraModeStack[0];
		if (BottomEntry.BlendTimeRemaining <= 0.0f && BottomEntry.CameraMode && BottomEntry.CameraMode->GetBlendWeight() <= 0.0f)
		{
			BottomEntry.CameraMode->OnDeactivate();
			CameraModeStack.RemoveAt(0);
		}
		else
		{
			break;
		}
	}

	// Evaluate the blended camera parameters
	// Start from the bottom and blend each mode on top
	URPGCameraMode* BaseMode = CameraModeStack[0].CameraMode;
	if (!BaseMode)
	{
		return;
	}

	// Get base mode parameters - use ViewLocation/ViewRotation from the mode
	FVector BaseLocation = BaseMode->ViewLocation;
	FRotator BaseRotation = BaseMode->ViewRotation;
	float BaseFOV = BaseMode->FieldOfView;

	OutCameraLocation = BaseLocation;
	OutCameraRotation = BaseRotation;
	OutFOV = BaseFOV;

	// Blend in any modes on top of the base
	for (int32 Index = 1; Index < CameraModeStack.Num(); ++Index)
	{
		FRPGCameraModeStackEntry& Entry = CameraModeStack[Index];
		URPGCameraMode* Mode = Entry.CameraMode;
		if (!Mode)
		{
			continue;
		}

		// Calculate blend alpha
		float BlendAlpha = 1.0f;
		if (Entry.TotalBlendTime > 0.0f)
		{
			const float ElapsedBlendTime = Entry.TotalBlendTime - Entry.BlendTimeRemaining;
			BlendAlpha = FMath::Clamp(ElapsedBlendTime / Entry.TotalBlendTime, 0.0f, 1.0f);

			// Apply blend function
			switch (Mode->BlendFunction)
			{
			case ERPGCameraModeBlendFunction::Linear:
				// Already linear
				break;
			case ERPGCameraModeBlendFunction::EaseIn:
				BlendAlpha = FMath::Pow(BlendAlpha, Mode->BlendExponent);
				break;
			case ERPGCameraModeBlendFunction::EaseOut:
				BlendAlpha = FMath::Pow(BlendAlpha, 1.0f / Mode->BlendExponent);
				break;
			case ERPGCameraModeBlendFunction::EaseInOut:
				if (BlendAlpha < 0.5f)
				{
					BlendAlpha = FMath::Pow(BlendAlpha * 2.0f, Mode->BlendExponent) * 0.5f;
				}
				else
				{
					BlendAlpha = 1.0f - FMath::Pow((1.0f - BlendAlpha) * 2.0f, Mode->BlendExponent) * 0.5f;
				}
				break;
			}
		}

		Mode->SetBlendAlpha(BlendAlpha);

		// Blend camera parameters using ViewLocation/ViewRotation from the mode
		OutCameraLocation = FMath::Lerp(OutCameraLocation, Mode->ViewLocation, BlendAlpha);
		OutCameraRotation = FMath::RInterpTo(OutCameraRotation, Mode->ViewRotation, BlendAlpha, 1.0f);
		OutFOV = FMath::Lerp(OutFOV, Mode->FieldOfView, BlendAlpha);
	}
}

URPGCameraMode* URPGCameraModeStack::CreateCameraMode(TSubclassOf<URPGCameraMode> CameraModeClass)
{
	if (!CameraModeClass)
	{
		return nullptr;
	}

	URPGCameraMode* NewMode = NewObject<URPGCameraMode>(GetTransientPackage(), CameraModeClass);
	if (NewMode)
	{
		NewMode->SetCameraComponent(CameraComponent);
	}

	return NewMode;
}

void URPGCameraModeStack::DrawDebug(UCanvas* Canvas) const
{
	for (const FRPGCameraModeStackEntry& Entry : CameraModeStack)
	{
		if (Entry.CameraMode)
		{
			Entry.CameraMode->DrawDebug(Canvas);
		}
	}
}

URPGCameraMode* URPGCameraModeStack::GetTopCameraMode() const
{
	if (CameraModeStack.Num() > 0)
	{
		return CameraModeStack.Last().CameraMode;
	}
	return nullptr;
}
