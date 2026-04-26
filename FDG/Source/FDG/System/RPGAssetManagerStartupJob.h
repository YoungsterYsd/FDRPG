// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/StreamableManager.h"

DECLARE_DELEGATE_OneParam(FRPGAssetManagerStartupJobSubstepProgress, float /*NewProgress*/);

/** Handles reporting progress from streamable handles */
struct FRPGAssetManagerStartupJob
{
	FRPGAssetManagerStartupJobSubstepProgress SubstepProgressDelegate;
	TFunction<void(const FRPGAssetManagerStartupJob&, TSharedPtr<FStreamableHandle>&)> JobFunc;
	FString JobName;
	float JobWeight;
	mutable double LastUpdate = 0;

	/** Simple job that is all synchronous */
	FRPGAssetManagerStartupJob(const FString& InJobName, const TFunction<void(const FRPGAssetManagerStartupJob&, TSharedPtr<FStreamableHandle>&)>& InJobFunc, float InJobWeight)
		: JobFunc(InJobFunc)
		, JobName(InJobName)
		, JobWeight(InJobWeight)
	{}

	/** Perform actual loading, will return a handle if it created one */
	TSharedPtr<FStreamableHandle> DoJob() const;

	void UpdateSubstepProgress(float NewProgress) const
	{
		SubstepProgressDelegate.ExecuteIfBound(NewProgress);
	}

	void UpdateSubstepProgressFromStreamable(TSharedRef<FStreamableHandle> StreamableHandle) const
	{
		if (SubstepProgressDelegate.IsBound())
		{
			// StreamableHandle::GetProgress traverses() a large graph and is quite expensive
			double Now = FPlatformTime::Seconds();
			if (LastUpdate - Now > 1.0 / 60)
			{
				SubstepProgressDelegate.Execute(StreamableHandle->GetProgress());
				LastUpdate = Now;
			}
		}
	}
};
