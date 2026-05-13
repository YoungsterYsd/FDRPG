// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"
#include "Widgets/SLeafWidget.h"

#include "RPGPerfStatWidgetBase.generated.h"

enum class ERPGDisplayablePerformanceStat : uint8;

class URPGPerformanceStatSubsystem;
class UObject;
struct FFrame;
class FSampledStatCache;

class SRPGLatencyGraph : public SLeafWidget
{
public:
	/** Begin the arguments for this slate widget */
	SLATE_BEGIN_ARGS(SRPGLatencyGraph)
		: _DesiredSize(150, 50),
		_MaxLatencyToGraph(33.0),
		_LineColor(255, 255, 255, 255),
		_BackgroundColor(0, 0, 0, 128)
	{
		_Clipping = EWidgetClipping::ClipToBounds;
	}

	SLATE_ARGUMENT(FVector2D, DesiredSize)
	SLATE_ARGUMENT(double, MaxLatencyToGraph)
	SLATE_ARGUMENT(FColor, LineColor)
	SLATE_ARGUMENT(FColor, BackgroundColor)
	SLATE_END_ARGS()

	/** Contruct function needed for every Widget */
	void Construct(const FArguments& InArgs);

	/** Called with the elements to be drawn */
	virtual int32 OnPaint(const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyClippingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

	virtual bool ComputeVolatility() const override { return true; }
	
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

	inline void SetLineColor(const FColor& InColor)
	{
		LineColor = InColor;	
	}

	inline void SetMaxYValue(const double InValue)
	{
		MaxYAxisOfGraph = InValue;
	}

	inline void SetBackgroundColor(const FColor& InColor)
	{
		BackgroundColor = InColor;
	}

	inline void UpdateGraphData(const FSampledStatCache* StatData, const float InScaleFactor)
	{
		GraphData = StatData;
		ScaleFactor = InScaleFactor;
	}
	
private:
	
	void DrawTotalLatency(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32 LayerId) const;

	/**
	 * The size of the graph to draw
	 */
	FVector2D DesiredSize = { 150.0, 50.0 };

	/**
	 * Max Y value of the graph. The values drawn will be clamped to this
	 */
	double MaxYAxisOfGraph = 33.0;

	float ScaleFactor = 1.0f;

	/**
	 * Color of the line to draw on the graph
	 */
	FColor LineColor = FColor(255, 255, 255, 255);

	/**
	 * The background color to draw when drawing the graph
	 */
	FColor BackgroundColor = FColor(0, 0, 0, 128);

	/**
	 * The cache of data that this graph widget needs to draw
	 */
	const FSampledStatCache* GraphData = nullptr;
};

/**
 * ULyraPerfStatGraph
 *
 * Base class for a widget that displays the graph of a stat over time.
 */
UCLASS(meta = (DisableNativeTick))
class URPGPerfStatGraph : public UUserWidget
{
	GENERATED_BODY()

public:
	URPGPerfStatGraph(const FObjectInitializer& ObjectInitializer);
	
	void SetLineColor(const FColor& InColor);
	
	void SetMaxYValue(const float InValue);
	
	void SetBackgroundColor(const FColor& InValue);

	void UpdateGraphData(const FSampledStatCache* StatData, const float ScaleFactor);
	
protected:
	// Begin UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// End UWidget interface

	// The actual slate widget which will draw the graph. Created in RebuildWidget and
	// destroyed in ReleaseSlateResources.
	TSharedPtr<SRPGLatencyGraph> SlateLatencyGraph;
};

/**
 * ULyraPerfStatWidgetBase
 *
 * Base class for a widget that displays a single stat, e.g., FPS, ping, etc...
 */
 UCLASS(Abstract)
class URPGPerfStatWidgetBase : public UCommonUserWidget
{
public:
	GENERATED_BODY()

public:
	// Returns the stat this widget is supposed to display
	UFUNCTION(BlueprintPure)
	ERPGDisplayablePerformanceStat GetStatToDisplay() const
	{
		return StatToDisplay;
	}

	// Polls for the value of this stat (unscaled)
	UFUNCTION(BlueprintPure)
	double FetchStatValue();

	UFUNCTION(BlueprintCallable)
	void UpdateGraphData(const float ScaleFactor = 1.0f);

protected:

 	virtual void NativeConstruct() override;

 	URPGPerformanceStatSubsystem* GetStatSubsystem();

	/**
	 * An optional stat graph widget to display this stat's value over time.
	 */
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget, OptionalWidget=true))
 	TObjectPtr<URPGPerfStatGraph> PerfStatGraph;
 	
	// Cached subsystem pointer
	UPROPERTY(Transient)
	TObjectPtr<URPGPerformanceStatSubsystem> CachedStatSubsystem;

 	UPROPERTY(EditAnywhere, Category = Display)
 	FColor GraphLineColor = FColor(255, 255, 255, 255);
	
 	UPROPERTY(EditAnywhere, Category = Display)
 	FColor GraphBackgroundColor = FColor(0, 0, 0, 128);

 	/**
	  * The max value of the Y axis to clamp the graph to. 
	  */
 	UPROPERTY(EditAnywhere, Category = Display)
 	double GraphMaxYValue = 33.0;

	// The stat to display
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Display)
	ERPGDisplayablePerformanceStat StatToDisplay;
 };
