#pragma once

#include "Abilities/GameplayAbility.h"

#include "RPGGameplayAbility.generated.h"

/**
 * ERPGAbilityActivationPolicy
 *
 * 定义能力的激活方式 —— 决定 ProcessAbilityInput 如何处理输入与激活的关系。
 *
 * - OnInputTriggered: 输入按下时尝试激活（如技能、攻击），最常用的模式
 * - WhileInputActive:  输入持续按住时每帧尝试激活（如移动、持续施法）
 * - OnSpawn:           能力被授予后自动激活（如被动技能）
 */
UENUM(BlueprintType)
enum class ERPGAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	WhileInputActive,
	OnSpawn
};


/**
 * ERPGAbilityActivationGroup
 *
 * 定义能力与其他能力的互斥关系 —— 控制"同时只能有一个排他性能力在运行"。
 *
 * ┌──────────────────────┬──────────────────────────────────────────────────┐
 * │ Independent          │ 独立运行，不影响其他能力（如被动、Buff）            │
 * │ Exclusive_Replaceable│ 可被 Blocking 组挤掉（如移动，攻击时自动取消）     │
 * │ Exclusive_Blocking   │ 阻断所有 Exclusive 组（如攻击、大招、硬直）        │
 * └──────────────────────┴──────────────────────────────────────────────────┘
 *
 * 规则：
 *   1. Independent 能力永远不会被阻断
 *   2. 当 Blocking 能力激活时，所有 Replaceable 能力被取消
 *   3. 同一时间最多只能有 1 个 Exclusive（Replaceable 或 Blocking）能力在运行
 */
UENUM(BlueprintType)
enum class ERPGAbilityActivationGroup : uint8
{
	Independent,
	Exclusive_Replaceable,
	Exclusive_Blocking,

	MAX	UMETA(Hidden)
};


/**
 * URPGGameplayAbility
 *
 * 本项目的 GameplayAbility 基类。
 * 最小化实现，仅包含 ASC 依赖的核心接口：
 *   - ActivationPolicy / ActivationGroup：驱动输入处理和互斥逻辑
 *   - TryActivateAbilityOnSpawn：支持 OnSpawn 策略的自动激活
 *   - OnPawnAvatarSet：通知能力 Avatar 已就绪
 *   - OnAbilityFailedToActivate：激活失败的回调通知
 */
UCLASS(MinimalAPI, Abstract, HideCategories = Input, Meta = (ShortTooltip = "RPG Gameplay Ability base class."))
class URPGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class URPGAbilitySystemComponent;

public:

	URPGGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 获取激活策略 */
	ERPGAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	/** 获取激活组 */
	ERPGAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }

	/**
	 * OnSpawn 策略的自动激活逻辑。
	 * 当 Policy == OnSpawn 时，在 ASC 的 InitAbilityActorInfo 中调用。
	 * 检查 CommitAbility（消耗/冷却）通过后尝试激活。
	 */
	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	/**
	 * 当 Avatar（Pawn）被设置时调用。
	 * 在 InitAbilityActorInfo 中，如果检测到新的 Pawn Avatar，会通知所有已授予的能力实例。
	 * 子类可 override 来执行 Avatar 依赖的初始化。
	 */
	virtual void OnPawnAvatarSet();

	/**
	 * 能力激活失败的回调。
	 * 由 ASC 的 HandleAbilityFailed 触发，通知能力自身失败原因。
	 * 子类可 override 来播放失败动画、UI 提示等。
	 */
	void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

protected:

	/** Native 层的激活失败回调，子类可 override */
	virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

	/** Blueprint 层的激活失败回调 */
	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityFailedToActivate")
	void ScriptOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

	//~UGameplayAbility interface
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	//~End of UGameplayAbility interface

protected:

	/** 定义此能力的激活方式（输入触发/持续按住/生成时自动） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Ability Activation")
	ERPGAbilityActivationPolicy ActivationPolicy = ERPGAbilityActivationPolicy::OnInputTriggered;

	/** 定义此能力与其他能力的互斥关系 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Ability Activation")
	ERPGAbilityActivationGroup ActivationGroup = ERPGAbilityActivationGroup::Independent;

	/** 此能力绑定的输入标签（用于输入触发） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RPG|Ability Activation")
	FGameplayTag AbilityInputTag;

public:
	/** 获取此能力的输入标签 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Ability")
	FGameplayTag GetAbilityInputTag() const { return AbilityInputTag; }
};
