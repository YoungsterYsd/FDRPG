// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSettingValueDiscrete.h"

#include "RPGSettingValueDiscrete_PerfStat.generated.h"

enum class ERPGDisplayablePerformanceStat : uint8;
enum class ERPGStatDisplayMode : uint8;

class UObject;

UCLASS()
class URPGSettingValueDiscrete_PerfStat : public UGameSettingValueDiscrete
{
	GENERATED_BODY()
	
public:

	URPGSettingValueDiscrete_PerfStat();

	void SetStat(ERPGDisplayablePerformanceStat InStat);

	/** UGameSettingValue */
	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

	/** UGameSettingValueDiscrete */
	virtual void SetDiscreteOptionByIndex(int32 Index) override;
	virtual int32 GetDiscreteOptionIndex() const override;
	virtual TArray<FText> GetDiscreteOptions() const override;

protected:
	/** UGameSettingValue */
	virtual void OnInitialized() override;
	
	void AddMode(FText&& Label, ERPGStatDisplayMode Mode);
protected:
	TArray<FText> Options;
	TArray<ERPGStatDisplayMode> DisplayModes;

	ERPGDisplayablePerformanceStat StatToDisplay;
	ERPGStatDisplayMode InitialMode;
};
