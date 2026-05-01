// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "RPGAbilitySystemComponent.generated.h"

class URPGGameplayAbility;

/** 能力目标数据追踪（P2 完整实现） */
struct FRPGGameplayTargetDataInfo
{
	FGameplayAbilitySpecHandle AbilityHandle;
	TArray<TSharedPtr<FGameplayAbilityTargetData>> TargetData;
};

/**
 * URPGAbilitySystemComponent
 *
 *	RPG 扩展的 AbilitySystemComponent。
 *	提供基于 InputTag 的能力激活、输入追踪和 ProcessAbilityInput 逐帧处理。
 *
 *	输入管线：InputAction -> OnAbilityInputPressed/Released -> ProcessAbilityInput -> TryActivateAbility
 */
UCLASS()
class RPGGAME_API URPGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	URPGAbilitySystemComponent();

	//~UAbilitySystemComponent interface
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	virtual void CancelAbilitiesByFunc(TFunctionRef<bool(const URPGGameplayAbility* Ability, FGameplayAbilitySpecHandle Handle)> ShouldCancelFunc,
		const FGameplayTagContainer* WithTags = nullptr, const FGameplayTagContainer* WithoutTags = nullptr, UGameplayAbility* IgnoreAbility = nullptr);
	//~End of UAbilitySystemComponent interface

	//~UActorComponent interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~End of UActorComponent interface

	// 输入按下/释放回调（由 RPGInputComponent 的 lambda 调用）
	void OnAbilityInputPressed(const FGameplayTag& InputTag);
	void OnAbilityInputReleased(const FGameplayTag& InputTag);

	// 逐帧处理能力输入（在 TickComponent 中调用）
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	// 根据 InputTag 查找 AbilitySpec
	FGameplayAbilitySpec* FindAbilitySpecForInputTag(const FGameplayTag& InputTag);

	// 获取指定能力的 TargetData（P2 实现）
	const FRPGGameplayTargetDataInfo* GetTargetDataInfoForAbility(FGameplayAbilitySpecHandle AbilityHandle) const;

	// 激活所有 OnSpawn 策略的能力
	void TryActivateAbilitiesOnSpawn();

	// 获取当前按住的第一个 InputTag
	FGameplayTag GetFirstPressedInputTag() const;

private:

	// 本帧按下输入的能力 Spec 句柄
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// 本帧释放输入的能力 Spec 句柄
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// 当前持续按住的能力 Spec 句柄
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	// 能力目标数据映射（P2 实现）
	TMap<FGameplayAbilitySpecHandle, FRPGGameplayTargetDataInfo> AbilityTargetDataMap;
};
