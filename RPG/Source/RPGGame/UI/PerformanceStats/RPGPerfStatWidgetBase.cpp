// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGPerfStatWidgetBase.h"

#include "Engine/GameInstance.h"
#include "Performance/RPGPerformanceStatSubsystem.h"
#include "Styling/CoreStyle.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGPerfStatWidgetBase)

void SRPGLatencyGraph::Construct(const FArguments& InArgs)
{
	DesiredSize = InArgs._DesiredSize;
	MaxYAxisOfGraph = InArgs._MaxLatencyToGraph;
	LineColor = InArgs._LineColor;
	BackgroundColor = InArgs._BackgroundColor;
}

int32 SRPGLatencyGraph::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect,
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 MaxLayerId = LayerId;

	// Draw the background
	FSlateDrawElement::MakeRotatedBox(
		OutDrawElements,
		MaxLayerId,
		AllottedGeometry.ToPaintGeometry(),
		FCoreStyle::Get().GetBrush("BlackBrush"),
		ESlateDrawEffect::NoPixelSnapping,
		0,
		TOptional<FVector2D>(),
		FSlateDrawElement::RelativeToElement,
		BackgroundColor);

	// We need to actually draw the graph plot on top of the background
	// so increment the layer
	MaxLayerId++;

	// Actually draw the graph plot
	DrawTotalLatency(AllottedGeometry, OutDrawElements, MaxLayerId);
	
	MaxLayerId++;

	return MaxLayerId;
}

FVector2D SRPGLatencyGraph::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return DesiredSize;
}

void SRPGLatencyGraph::DrawTotalLatency(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32 LayerId) const
{
	if (!GraphData)
	{
		return;
	}
	
	static TArray<FVector2D> Points;
	Points.Reset(GraphData->GetSampleSize() + 1);
	
	const FVector2D WidgetSize = AllottedGeometry.GetLocalSize();
	const float LineThickness = 1.0f;
	const double XSlice = WidgetSize.X / static_cast<double>(GraphData->GetSampleSize());
	const double Border = 1.0;

	int32 i = 0;
	
	GraphData->ForEachCurrentSample([&](const double Stat)
	{
		double Y = WidgetSize.Y - FMath::Clamp((Stat * ScaleFactor), 0.0, MaxYAxisOfGraph) / MaxYAxisOfGraph * WidgetSize.Y;
		Y = FMath::Clamp(Y, Border, WidgetSize.Y - Border);

		Points.Emplace(XSlice * double(++i), Y);
	});

	// Why does this not just draw a straight line?? 
	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(),
		Points,
		ESlateDrawEffect::NoPixelSnapping,
		LineColor,
		false,
		LineThickness);
}

//////////////////////////////////////////////////////////////////////
// ULyraPerfStatGraph

URPGPerfStatGraph::URPGPerfStatGraph(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetClipping(EWidgetClipping::ClipToBounds);
}

TSharedRef<SWidget> URPGPerfStatGraph::RebuildWidget()
{
	return SAssignNew(SlateLatencyGraph, SRPGLatencyGraph);
}

void URPGPerfStatGraph::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	SlateLatencyGraph.Reset();
}

void URPGPerfStatGraph::SetLineColor(const FColor& InColor)
{
	SlateLatencyGraph->SetLineColor(InColor);
}

void URPGPerfStatGraph::SetMaxYValue(const float InValue)
{
	SlateLatencyGraph->SetMaxYValue(InValue);
}

void URPGPerfStatGraph::SetBackgroundColor(const FColor& InValue)
{
	SlateLatencyGraph->SetBackgroundColor(InValue);
}

void URPGPerfStatGraph::UpdateGraphData(const FSampledStatCache* StatData, const float ScaleFactor)
{
	SlateLatencyGraph->UpdateGraphData(StatData, ScaleFactor);
}

//////////////////////////////////////////////////////////////////////
// ULyraPerfStatWidgetBase

double URPGPerfStatWidgetBase::FetchStatValue()
{
	if (URPGPerformanceStatSubsystem* Subsystem = GetStatSubsystem())
	{
		return CachedStatSubsystem->GetCachedStat(StatToDisplay);
	}
	else
	{
		return 0.0;
	}
}

void URPGPerfStatWidgetBase::UpdateGraphData(const float ScaleFactor)
{
	// When we cache the subsystem also update the graph data pointer if we have a graph widget
	if (PerfStatGraph)
	{
		if (const FSampledStatCache* GraphData = CachedStatSubsystem->GetCachedStatData(StatToDisplay))
		{
			PerfStatGraph->UpdateGraphData(GraphData, ScaleFactor);
		}	
	}
}

void URPGPerfStatWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache the subsystem on construct, which will also make sure the graph is up to date
	GetStatSubsystem();
	
	if (PerfStatGraph)
	{
		PerfStatGraph->SetLineColor(GraphLineColor);
		PerfStatGraph->SetMaxYValue(GraphMaxYValue);
		PerfStatGraph->SetBackgroundColor(GraphBackgroundColor);
	}
}

URPGPerformanceStatSubsystem* URPGPerfStatWidgetBase::GetStatSubsystem()
{
	if (CachedStatSubsystem == nullptr)
	{
		if (UWorld* World = GetWorld())
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				CachedStatSubsystem = GameInstance->GetSubsystem<URPGPerformanceStatSubsystem>();
			}
		}
	}

	return CachedStatSubsystem;
}
