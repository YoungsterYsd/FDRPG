#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "RPGGameplayAbility.h"
#include "NativeGameplayTags.h"
#include "RPGAbilitySystemComponent.generated.h"

class AActor;
class UGameplayAbility;
class URPGAbilityTagRelationshipMapping;
class UObject;
struct FFrame;
struct FGameplayAbilityTargetDataHandle;

/** 输入阻断标签 —— 当 ASC 拥有此标签时，ProcessAbilityInput 会清空所有输入缓存并跳过处理 */
RPG_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_AbilityInputBlocked);

/**
 * URPGAbilitySystemComponent
 *
 * 本项目的 AbilitySystemComponent 基类，基于 Lyra 架构。
 *
 * 核心功能：
 * ┌────────────────────────────────────────────────────────────────────┐
 * │ 1. 输入处理                                                        │
 * │    AbilityInputTagPressed/Released → 缓存 SpecHandle              │
 * │    ProcessAbilityInput(每帧) → 根据 ActivationPolicy 决定激活/传递  │
 * │                                                                    │
 * │ 2. 激活组管理 (ActivationGroup)                                    │
 * │    控制 Exclusive 能力的互斥：攻击自动取消移动，大招阻断其他技能      │
 * │                                                                    │
 * │ 3. Tag 关系映射 (TagRelationshipMapping)                           │
 * │    数据驱动的标签互斥/依赖关系表，自动扩展 Block/Cancel/激活约束     │
 * │                                                                    │
 * │ 4. 动态标签 GE                                                     │
 * │    通过 GameplayEffect 临时授予/移除标签，无需创建专门 GE 资产       │
 * └────────────────────────────────────────────────────────────────────┘
 */
UCLASS()
class RPG_API URPGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	URPGAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	/**
	 * 按条件取消能力。
	 * 遍历所有活跃能力实例，对满足 ShouldCancelFunc 的调用 CancelAbility。
	 * 这是 CancelInputActivatedAbilities 和 CancelActivationGroupAbilities 的底层实现。
	 */
	typedef TFunctionRef<bool(const URPGGameplayAbility* RPGAbility, FGameplayAbilitySpecHandle Handle)> TShouldCancelAbilityFunc;
	void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);

	/** 取消所有由输入激活的能力（ActivationPolicy 为 OnInputTriggered 或 WhileInputActive 的能力） */
	void CancelInputActivatedAbilities(bool bReplicateCancelAbility);

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	/**
	 * 每帧处理能力输入（由 PlayerController::PostProcessInput 调用）。
	 * 核心流程：
	 *   1. 检查输入阻断标签，有则清空所有输入
	 *   2. 处理 Held 输入 → WhileInputActive 策略的激活
	 *   3. 处理 Pressed 输入 → OnInputTriggered 策略的激活 + 已激活能力的 InputPressed 事件
	 *   4. 批量 TryActivate
	 *   5. 处理 Released 输入 → 已激活能力的 InputReleased 事件
	 */
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	//~ 激活组管理 ──────────────────────────────────────────────────
	// 控制 Exclusive 能力的互斥关系：
	//   Independent:       不受影响
	//   Exclusive_Replaceable: 可被 Blocking 挤掉
	//   Exclusive_Blocking:    阻断所有 Exclusive 组

	/** 检查指定激活组是否被阻断（有 Blocking 能力运行时，Replaceable 和 Blocking 组被阻断） */
	bool IsActivationGroupBlocked(ERPGAbilityActivationGroup Group) const;

	/** 将能力添加到激活组，自动取消低优先级组的能力 */
	void AddAbilityToActivationGroup(ERPGAbilityActivationGroup Group, URPGGameplayAbility* RPGAbility);

	/** 将能力从激活组中移除（能力结束时调用） */
	void RemoveAbilityFromActivationGroup(ERPGAbilityActivationGroup Group, URPGGameplayAbility* RPGAbility);

	/** 取消指定激活组的所有能力（排除 IgnoreAbility） */
	void CancelActivationGroupAbilities(ERPGAbilityActivationGroup Group, URPGGameplayAbility* IgnoreRPGAbility, bool bReplicateCancelAbility);
	//~ ─────────────────────────────────────────────────────────────

	/**
	 * 通过 GameplayEffect 动态添加标签。
	 * 使用 DynamicTagGameplayEffect 属性配置的 GE 类，在其 DynamicGrantedTags 中添加指定标签。
	 * 适用场景：临时授予状态标签（如"无敌"、"霸体"等）
	 */
	void AddDynamicTagGameplayEffect(const FGameplayTag& Tag);

	/** 移除通过 AddDynamicTagGameplayEffect 添加的所有匹配标签的 GE 实例 */
	void RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag);

	/** 获取指定能力的 TargetData（用于网络复制） */
	void GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle);

	/** 设置 Tag 关系映射表，null 则清除 */
	void SetTagRelationshipMapping(URPGAbilityTagRelationshipMapping* NewMapping);

	/**
	 * 查询 Tag 关系映射表，获取额外的激活必需/阻断标签。
	 * 由 GAS 内部在检查能力激活条件时调用。
	 */
	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const;

	/** 尝试激活所有 OnSpawn 策略的能力（在 InitAbilityActorInfo 中自动调用） */
	void TryActivateAbilitiesOnSpawn();

	/**
	 * 将 InputTag 添加到指定 AbilitySpec 的 DynamicTags 中
	 * 这样 AbilityInputTagPressed/Released 才能匹配到该能力
	 * @param SpecHandle 能力Spec句柄
	 * @param InputTag 要绑定的输入标签
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Ability")
	void AddInputTagToAbilitySpec(const FGameplayAbilitySpecHandle& SpecHandle, const FGameplayTag& InputTag);

protected:

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;
	virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason) override;
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;

	/**
	 * 重写以支持 TagRelationshipMapping 扩展。
	 * 当能力激活时，查映射表扩展 Block/Cancel 标签范围，
	 * 然后调用 Super 执行实际的阻断和取消逻辑。
	 */
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;
	virtual void HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled) override;

	/** 客户端 RPC：通知能力激活失败（服务端检测到失败后转发给客户端） */
	UFUNCTION(Client, Unreliable)
	void ClientNotifyAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);

	/** 处理能力激活失败：调用 RPGGameplayAbility::OnAbilityFailedToActivate */
	void HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);

protected:

	/** Tag 关系映射表，null 时不添加额外约束 */
	UPROPERTY()
	TObjectPtr<URPGAbilityTagRelationshipMapping> TagRelationshipMapping;

	/**
	 * 动态标签 GameplayEffect 类。
	 * AddDynamicTagGameplayEffect 使用此 GE 的 DynamicGrantedTags 来临时添加标签。
	 * 需要在蓝图中配置为一个 Duration=Infinite 的 GE（无 Modifier，仅 Grant Tags）。
	 *
	 * TODO: 后续接入 AssetManager/GameData 后可改为自动查找，此处 UPROPERTY 仅作过渡方案。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "RPG|AbilitySystem")
	TSubclassOf<UGameplayEffect> DynamicTagGameplayEffect;

	// ── 输入缓存 ──────────────────────────────────────────────
	// 这些数组在 ProcessAbilityInput 每帧中被读写和清空，
	// 使用 Reset() 而非 Empty() 以避免内存重分配。

	/** 本帧按下输入的能力 SpecHandle */
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	/** 本帧释放输入的能力 SpecHandle */
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	/** 当前输入持续按住的能力 SpecHandle */
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	// ── 激活组计数 ────────────────────────────────────────────
	// 用 int32 计数追踪每个激活组中正在运行的能力数量，
	// 索引对应 ERPGAbilityActivationGroup 枚举值。

	/** 每个激活组中的活跃能力数量 */
	int32 ActivationGroupCounts[(uint8)ERPGAbilityActivationGroup::MAX];
};
