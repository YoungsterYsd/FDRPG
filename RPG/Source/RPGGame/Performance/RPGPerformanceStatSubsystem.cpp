// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGPerformanceStatSubsystem.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/NetConnection.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "GameModes/RPGGameState.h"
#include "Performance/RPGPerformanceStatTypes.h"
#include "Performance/LatencyMarkerModule.h"
#include "ProfilingDebugging/CsvProfiler.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGPerformanceStatSubsystem)

CSV_DEFINE_CATEGORY(RPGPerformance, /*bIsEnabledByDefault=*/false);

class FSubsystemCollectionBase;

//////////////////////////////////////////////////////////////////////
// FLyraPerformanceStatCache

void FRPGPerformanceStatCache::StartCharting()
{
}

void FRPGPerformanceStatCache::ProcessFrame(const FFrameData& FrameData)
{
	// Record stats about the frame data
	{
		RecordStat(
			ERPGDisplayablePerformanceStat::ClientFPS,
			(FrameData.TrueDeltaSeconds != 0.0) ?
			1.0 / FrameData.TrueDeltaSeconds :
			0.0);

		RecordStat(ERPGDisplayablePerformanceStat::IdleTime, FrameData.IdleSeconds);
		RecordStat(ERPGDisplayablePerformanceStat::FrameTime, FrameData.TrueDeltaSeconds);
		RecordStat(ERPGDisplayablePerformanceStat::FrameTime_GameThread, FrameData.GameThreadTimeSeconds);
		RecordStat(ERPGDisplayablePerformanceStat::FrameTime_RenderThread, FrameData.RenderThreadTimeSeconds);
		RecordStat(ERPGDisplayablePerformanceStat::FrameTime_RHIThread, FrameData.RHIThreadTimeSeconds);
		RecordStat(ERPGDisplayablePerformanceStat::FrameTime_GPU, FrameData.GPUTimeSeconds);	
	}

	if (UWorld* World = MySubsystem->GetGameInstance()->GetWorld())
	{
		// Record some networking related stats
		if (const ARPGGameState* GameState = World->GetGameState<ARPGGameState>())
		{
			RecordStat(ERPGDisplayablePerformanceStat::ServerFPS, GameState->GetServerFPS());
		}

		if (APlayerController* LocalPC = GEngine->GetFirstLocalPlayerController(World))
		{
			if (APlayerState* PS = LocalPC->GetPlayerState<APlayerState>())
			{
				RecordStat(ERPGDisplayablePerformanceStat::Ping, PS->GetPingInMilliseconds());
			}

			if (UNetConnection* NetConnection = LocalPC->GetNetConnection())
			{
				const UNetConnection::FNetConnectionPacketLoss& InLoss = NetConnection->GetInLossPercentage();
				RecordStat(ERPGDisplayablePerformanceStat::PacketLoss_Incoming, InLoss.GetAvgLossPercentage());
				
				const UNetConnection::FNetConnectionPacketLoss& OutLoss = NetConnection->GetOutLossPercentage();
				RecordStat(ERPGDisplayablePerformanceStat::PacketLoss_Outgoing, OutLoss.GetAvgLossPercentage());
				
				RecordStat(ERPGDisplayablePerformanceStat::PacketRate_Incoming, NetConnection->InPacketsPerSecond);
				RecordStat(ERPGDisplayablePerformanceStat::PacketRate_Outgoing, NetConnection->OutPacketsPerSecond);

				RecordStat(ERPGDisplayablePerformanceStat::PacketSize_Incoming, (NetConnection->InPacketsPerSecond != 0) ? (NetConnection->InBytesPerSecond / (float)NetConnection->InPacketsPerSecond) : 0.0f);
				RecordStat(ERPGDisplayablePerformanceStat::PacketSize_Outgoing, (NetConnection->OutPacketsPerSecond != 0) ? (NetConnection->OutBytesPerSecond / (float)NetConnection->OutPacketsPerSecond) : 0.0f);
			}
			
			// Finally, record some input latency related stats if they are enabled
			TArray<ILatencyMarkerModule*> LatencyMarkerModules = IModularFeatures::Get().GetModularFeatureImplementations<ILatencyMarkerModule>(ILatencyMarkerModule::GetModularFeatureName());
			for (ILatencyMarkerModule* LatencyMarkerModule : LatencyMarkerModules)
			{
				if (LatencyMarkerModule->GetEnabled())
				{
					const float TotalLatencyMs = LatencyMarkerModule->GetTotalLatencyInMs();
					if (TotalLatencyMs > 0.0f)
					{
						// Record some stats about the latency of the game
						RecordStat(ERPGDisplayablePerformanceStat::Latency_Total, TotalLatencyMs);
						RecordStat(ERPGDisplayablePerformanceStat::Latency_Game, LatencyMarkerModule->GetGameLatencyInMs());
						RecordStat(ERPGDisplayablePerformanceStat::Latency_Render, LatencyMarkerModule->GetRenderLatencyInMs());

						// Record some CSV profile stats.
						// You can see these by using the following commands
						// Start and stop the profile:
						//	CsvProfile Start
						//	CsvProfile Stop
						//
						// Or, you can profile for a certain number of frames:
						// CsvProfile Frames=10
						//
						// And this will output a .csv file to the Saved\Profiling\CSV folder
#if CSV_PROFILER
						if (FCsvProfiler* Profiler = FCsvProfiler::Get())
						{
							static const FName TotalLatencyStatName = TEXT("Lyra_Latency_Total");
							Profiler->RecordCustomStat(TotalLatencyStatName, CSV_CATEGORY_INDEX(RPGPerformance), TotalLatencyMs, ECsvCustomStatOp::Set);

							static const FName GameLatencyStatName = TEXT("Lyra_Latency_Game");
							Profiler->RecordCustomStat(GameLatencyStatName, CSV_CATEGORY_INDEX(RPGPerformance), LatencyMarkerModule->GetGameLatencyInMs(), ECsvCustomStatOp::Set);

							static const FName RenderLatencyStatName = TEXT("Lyra_Latency_Render");
							Profiler->RecordCustomStat(RenderLatencyStatName, CSV_CATEGORY_INDEX(RPGPerformance), LatencyMarkerModule->GetRenderLatencyInMs(), ECsvCustomStatOp::Set);
						}
#endif

						// Some more fine grain latency numbers can be found on the marker module if desired
						//LatencyMarkerModule->GetRenderLatencyInMs()));
						//LatencyMarkerModule->GetDriverLatencyInMs()));
						//LatencyMarkerModule->GetOSRenderQueueLatencyInMs()));
						//LatencyMarkerModule->GetGPURenderLatencyInMs()));
						break;	
					}
				}
			}
		}
	}
}

void FRPGPerformanceStatCache::StopCharting()
{
}

void FRPGPerformanceStatCache::RecordStat(const ERPGDisplayablePerformanceStat Stat, const double Value)
{
	PerfStateCache.FindOrAdd(Stat).RecordSample(Value);
}

double FRPGPerformanceStatCache::GetCachedStat(ERPGDisplayablePerformanceStat Stat) const
{
	static_assert((int32)ERPGDisplayablePerformanceStat::Count == 18, "Need to update this function to deal with new performance stats");

	if (const FSampledStatCache* Cache = GetCachedStatData(Stat))
	{
		return Cache->GetLastCachedStat();
	}

	return 0.0;
}

const FSampledStatCache* FRPGPerformanceStatCache::GetCachedStatData(const ERPGDisplayablePerformanceStat Stat) const
{
	static_assert((int32)ERPGDisplayablePerformanceStat::Count == 18, "Need to update this function to deal with new performance stats");
	
	return PerfStateCache.Find(Stat);
}

//////////////////////////////////////////////////////////////////////
// ULyraPerformanceStatSubsystem

void URPGPerformanceStatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Tracker = MakeShared<FRPGPerformanceStatCache>(this);
	GEngine->AddPerformanceDataConsumer(Tracker);
}

void URPGPerformanceStatSubsystem::Deinitialize()
{
	GEngine->RemovePerformanceDataConsumer(Tracker);
	Tracker.Reset();
}

double URPGPerformanceStatSubsystem::GetCachedStat(ERPGDisplayablePerformanceStat Stat) const
{
	return Tracker->GetCachedStat(Stat);
}

const FSampledStatCache* URPGPerformanceStatSubsystem::GetCachedStatData(const ERPGDisplayablePerformanceStat Stat) const
{
	return Tracker->GetCachedStatData(Stat);
}

