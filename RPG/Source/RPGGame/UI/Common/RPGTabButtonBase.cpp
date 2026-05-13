// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGTabButtonBase.h"

#include "CommonLazyImage.h"
#include "UI/Common/RPGTabListWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGTabButtonBase)

class UObject;
struct FSlateBrush;

void URPGTabButtonBase::SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrushFromLazyDisplayAsset(LazyObject);
	}
}

void URPGTabButtonBase::SetIconBrush(const FSlateBrush& Brush)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrush(Brush);
	}
}

void URPGTabButtonBase::SetTabLabelInfo_Implementation(const FRPGTabDescriptor& TabLabelInfo)
{
	SetButtonText(TabLabelInfo.TabText);
	SetIconBrush(TabLabelInfo.IconBrush);
}

