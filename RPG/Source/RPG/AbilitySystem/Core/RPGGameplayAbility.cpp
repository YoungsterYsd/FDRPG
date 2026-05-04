#include "RPGGameplayAbility.h"
#include "RPGAbilitySystemComponent.h"
#include "../../System/RPGLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGGameplayAbility)

URPGGameplayAbility::URPGGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URPGGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	// 仅当激活策略为 OnSpawn（生成时自动激活）时才执行
	if (ActivationPolicy != ERPGAbilityActivationPolicy::OnSpawn)
	{
		return;
	}

	// 检查是否满足激活条件（标签、消耗、冷却等）
	if (!Spec.IsActive() && CanActivateAbility(Spec.Handle, ActorInfo, nullptr, nullptr, nullptr))
	{
		// TryActivateAbility 是 UAbilitySystemComponent 的方法，需要通过 ASC 调用
		// （不是 UGameplayAbility 的方法）
		if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
		{
			ASC->TryActivateAbility(Spec.Handle, false);
		}
	}
}

void URPGGameplayAbility::OnPawnAvatarSet()
{
	// 空实现，供子类 override。
	// 在 ASC::InitAbilityActorInfo 中检测到新 Avatar 时调用。
	// 典型用途：绑定 Avatar 依赖的委托、初始化动画等。
}

void URPGGameplayAbility::OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
{
	// 先调用 Native 回调（C++ 子类 override）
	NativeOnAbilityFailedToActivate(FailedReason);
	// 再调用 Blueprint 回调（蓝图子类实现）
	ScriptOnAbilityFailedToActivate(FailedReason);
}

void URPGGameplayAbility::NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
{
	// 默认空实现，子类可 override 处理失败逻辑（如播放失败动画、显示 UI 提示等）
}

bool URPGGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	// 先检查基类条件（标签、冷却、消耗等）
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// 检查激活组是否被阻断：
	// 如果当前有 Exclusive_Blocking 能力正在运行，则 Exclusive_Replaceable/Blocking 组的能力无法激活
	if (URPGAbilitySystemComponent* ASC = Cast<URPGAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get()))
	{
		if (ASC->IsActivationGroupBlocked(ActivationGroup))
		{
			return false;
		}
	}

	return true;
}
