// Copyright Epic Games, Inc. All Rights Reserved.

#include "System/RPGAssetManagerStartupJob.h"
#include "System/RPGLogChannels.h"

TSharedPtr<FStreamableHandle> FRPGAssetManagerStartupJob::DoJob() const
{
	const double JobStartTime = FPlatformTime::Seconds();

	TSharedPtr<FStreamableHandle> Handle;
	UE_LOG(LogRPG, Display, TEXT("Startup job \"%s\" starting"), *JobName);
	JobFunc(*this, Handle);

	if (Handle.IsValid())
	{
		Handle->BindUpdateDelegate(FStreamableUpdateDelegate::CreateRaw(this, &FRPGAssetManagerStartupJob::UpdateSubstepProgressFromStreamable));
		Handle->WaitUntilComplete(0.0f, false);
		Handle->BindUpdateDelegate(FStreamableUpdateDelegate());
	}

	UE_LOG(LogRPG, Display, TEXT("Startup job \"%s\" took %.2f seconds to complete"), *JobName, FPlatformTime::Seconds() - JobStartTime);

	return Handle;
}
