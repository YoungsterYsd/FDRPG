// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RPGTabListWidgetBase.h"
#include "UI/Foundation/RPGButtonBase.h"

#include "RPGTabButtonBase.generated.h"

#define UE_API RPGGAME_API

class UCommonLazyImage;
class UObject;
struct FFrame;
struct FSlateBrush;

UCLASS(MinimalAPI, Abstract, Blueprintable, meta = (DisableNativeTick))
class URPGTabButtonBase : public URPGButtonBase, public IRPGTabButtonInterface
{
	GENERATED_BODY()

public:

	UE_API void SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject);
	UE_API void SetIconBrush(const FSlateBrush& Brush);

protected:

	UFUNCTION()
	UE_API virtual void SetTabLabelInfo_Implementation(const FRPGTabDescriptor& TabLabelInfo) override;

private:

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonLazyImage> LazyImage_Icon;
};

#undef UE_API
