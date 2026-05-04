#include "RPGAbilitySystemComponent.h"
#include "RPGGameplayAbility.h"
#include "RPGAbilityTagRelationshipMapping.h"
#include "../../System/RPGLogChannels.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGAbilitySystemComponent)

// ============================================================================
// 输入阻断标签定义
// 当 ASC 拥有此标签时（如眩晕、过场动画），ProcessAbilityInput 会跳过所有输入处理
// ============================================================================
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_AbilityInputBlocked, "Gameplay.AbilityInputBlocked");

// ============================================================================
// 构造函数
// ============================================================================
URPGAbilitySystemComponent::URPGAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();

	// 初始化激活组计数器为 0
	FMemory::Memset(ActivationGroupCounts, 0, sizeof(ActivationGroupCounts));
}

// ============================================================================
// EndPlay — 组件销毁时的清理
// ============================================================================
void URPGAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// TODO: 后续接入 RPGGlobalAbilitySystem 后，在此注销 ASC
	// if (URPGGlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<URPGGlobalAbilitySystem>(GetWorld()))
	// {
	// 	GlobalAbilitySystem->UnregisterASC(this);
	// }

	Super::EndPlay(EndPlayReason);
}

// ============================================================================
// InitAbilityActorInfo — ASC 初始化核心入口
//
// 调用时机：PlayerState::PostInitializeComponents 或手动调用
// 关键逻辑：
//   1. 调用 Super 初始化 ActorInfo
//   2. 如果是新 Pawn Avatar，通知所有已授予能力的实例（OnPawnAvatarSet）
//   3. 注册到全局能力系统（TODO）
//   4. 尝试激活 OnSpawn 策略的能力
// ============================================================================
void URPGAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	// 判断是否是新 Pawn Avatar（用于决定是否需要通知能力实例）
	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);

	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (bHasNewPawnAvatar)
	{
		// 通知所有已授予能力的新实例：Avatar 已就绪
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
PRAGMA_DISABLE_DEPRECATION_WARNINGS
			// 确保所有能力使用实例化策略（NonInstanced 已废弃）
			ensureMsgf(AbilitySpec.Ability && AbilitySpec.Ability->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced,
				TEXT("InitAbilityActorInfo: All Abilities should be Instanced."));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
	
			// 获取能力的所有实例，通知每个实例
			TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
			for (UGameplayAbility* AbilityInstance : Instances)
			{
				if (URPGGameplayAbility* RPGAbilityInstance = Cast<URPGGameplayAbility>(AbilityInstance))
				{
					RPGAbilityInstance->OnPawnAvatarSet();
				}
			}
		}

		// TODO: 后续接入 RPGGlobalAbilitySystem 后，在此注册 ASC
		// if (URPGGlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<URPGGlobalAbilitySystem>(GetWorld()))
		// {
		// 	GlobalAbilitySystem->RegisterASC(this);
		// }

		// TODO: 后续接入 RPGAnimInstance 后，在此初始化动画系统
		// if (URPGAnimInstance* RPGAnimInst = Cast<URPGAnimInstance>(ActorInfo->GetAnimInstance()))
		// {
		// 	RPGAnimInst->InitializeWithAbilitySystem(this);
		// }

		// 尝试激活所有 OnSpawn 策略的能力
		TryActivateAbilitiesOnSpawn();
	}
}

// ============================================================================
// TryActivateAbilitiesOnSpawn — 激活所有 OnSpawn 策略的能力
//
// 遍历所有已授予的能力，如果 ActivationPolicy == OnSpawn，则尝试激活。
// 典型应用：被动技能、初始 Buff 等
// ============================================================================
void URPGAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (const URPGGameplayAbility* RPGAbilityCDO = Cast<URPGGameplayAbility>(AbilitySpec.Ability))
		{
			RPGAbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
		}
	}
}

// ============================================================================
// AddInputTagToAbilitySpec — 将 InputTag 添加到 AbilitySpec 的 DynamicTags
//
// 这样 AbilityInputTagPressed/Released 才能通过 DynamicSpecSourceTags 匹配到该能力
// ============================================================================
void URPGAbilitySystemComponent::AddInputTagToAbilitySpec(const FGameplayAbilitySpecHandle& SpecHandle, const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		UE_LOG(LogRPGAbilitySystem, Warning, TEXT("AddInputTagToAbilitySpec: Invalid InputTag."));
		return;
	}

	FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle);
	if (!Spec)
	{
		UE_LOG(LogRPGAbilitySystem, Warning, TEXT("AddInputTagToAbilitySpec: SpecHandle not found."));
		return;
	}

	// 添加到 DynamicTags（会在 GetDynamicSpecSourceTags 中返回）
	Spec->GetDynamicSpecSourceTags().AddTag(InputTag);

	UE_LOG(LogRPGAbilitySystem, Display, TEXT("AddInputTagToAbilitySpec: Added InputTag [%s] to Ability [%s]"),
		*InputTag.ToString(), *GetNameSafe(Spec->Ability));
}

// ============================================================================
// CancelAbilitiesByFunc — 按条件取消能力的底层实现
//
// 遍历所有活跃能力实例，对满足 ShouldCancelFunc 的调用 CancelAbility。
// 这是 CancelInputActivatedAbilities 和 CancelActivationGroupAbilities 的公共逻辑。
// ============================================================================
void URPGAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility)
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.IsActive())
		{
			continue;
		}

		URPGGameplayAbility* RPGAbilityCDO = Cast<URPGGameplayAbility>(AbilitySpec.Ability);
		if (!RPGAbilityCDO)
		{
			UE_LOG(LogRPGAbilitySystem, Error, TEXT("CancelAbilitiesByFunc: Non-RPGGameplayAbility [%s] was Granted to ASC. Skipping."), *AbilitySpec.Ability.GetName());
			continue;
		}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
		ensureMsgf(AbilitySpec.Ability->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced,
			TEXT("CancelAbilitiesByFunc: All Abilities should be Instanced."));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
			
		// 取消所有实例化的能力实例
		TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
		for (UGameplayAbility* AbilityInstance : Instances)
		{
			URPGGameplayAbility* RPGAbilityInstance = CastChecked<URPGGameplayAbility>(AbilityInstance);

			if (ShouldCancelFunc(RPGAbilityInstance, AbilitySpec.Handle))
			{
				if (RPGAbilityInstance->CanBeCanceled())
				{
					RPGAbilityInstance->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), RPGAbilityInstance->GetCurrentActivationInfo(), bReplicateCancelAbility);
				}
				else
				{
					UE_LOG(LogRPGAbilitySystem, Error, TEXT("CancelAbilitiesByFunc: Can't cancel ability [%s] because CanBeCanceled is false."), *RPGAbilityInstance->GetName());
				}
			}
		}
	}
}

// ============================================================================
// CancelInputActivatedAbilities — 取消所有由输入激活的能力
//
// 取消 ActivationPolicy 为 OnInputTriggered 或 WhileInputActive 的能力。
// 典型场景：角色死亡时取消所有玩家可控制的能力
// ============================================================================
void URPGAbilitySystemComponent::CancelInputActivatedAbilities(bool bReplicateCancelAbility)
{
	auto ShouldCancelFunc = [this](const URPGGameplayAbility* RPGAbility, FGameplayAbilitySpecHandle Handle)
	{
		const ERPGAbilityActivationPolicy ActivationPolicy = RPGAbility->GetActivationPolicy();
		return ((ActivationPolicy == ERPGAbilityActivationPolicy::OnInputTriggered) || (ActivationPolicy == ERPGAbilityActivationPolicy::WhileInputActive));
	};

	CancelAbilitiesByFunc(ShouldCancelFunc, bReplicateCancelAbility);
}

// ============================================================================
// AbilitySpecInputPressed / Released — 输入事件的底层转发
//
// 不使用 bReplicateInputDirectly，而是通过 InvokeReplicatedEvent 将输入事件
// 同步到服务端。这样 WaitInputPress/WaitInputRelease 等 AbilityTask 才能正常工作。
// ============================================================================
void URPGAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	if (Spec.IsActive())
	{
PRAGMA_DISABLE_DEPRECATION_WARNINGS
		const UGameplayAbility* Instance = Spec.GetPrimaryInstance();
		FPredictionKey OriginalPredictionKey = Instance ? Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : Spec.ActivationInfo.GetActivationPredictionKey();
PRAGMA_ENABLE_DEPRECATION_WARNINGS

		// 触发 InputPressed 复制事件，WaitInputPress Task 会收到通知
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, OriginalPredictionKey);
	}
}

void URPGAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (Spec.IsActive())
	{
PRAGMA_DISABLE_DEPRECATION_WARNINGS
		const UGameplayAbility* Instance = Spec.GetPrimaryInstance();
		FPredictionKey OriginalPredictionKey = Instance ? Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : Spec.ActivationInfo.GetActivationPredictionKey();
PRAGMA_ENABLE_DEPRECATION_WARNINGS

		// 触发 InputReleased 复制事件，WaitInputRelease Task 会收到通知
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, OriginalPredictionKey);
	}
}

// ============================================================================
// AbilityInputTagPressed / Released — 输入标签 → SpecHandle 缓存
//
// 由 HeroComponent 或 PlayerController 在输入事件时调用。
// 不直接激活能力，而是将 SpecHandle 缓存到对应的数组中，
// 等待 ProcessAbilityInput 统一处理。
//
// 这种"缓存-批量处理"设计的好处：
//   1. 同一帧内的多个输入可以合并处理，避免冲突
//   2. 便于检查输入阻断标签（如眩晕时一键清空所有输入）
// ============================================================================
void URPGAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	UE_LOG(LogRPG, Display, TEXT("AbilityInputTagPressed: InputTag = [%s]"), *InputTag.ToString());
	
	if (InputTag.IsValid())
	{
		UE_LOG(LogRPG, Display, TEXT("AbilityInputTagPressed: Searching %d abilities..."), ActivatableAbilities.Items.Num());
		
		int32 FoundCount = 0;
		// 遍历所有能力，找到 DynamicSpecSourceTags 中匹配 InputTag 的能力
		// （InputTag 是通过 RPGAbilitySet 授予能力时绑定到 Spec 的 DynamicTags 中的）
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				UE_LOG(LogRPG, Display, TEXT("AbilityInputTagPressed: Found matching ability [%s]"), *GetNameSafe(AbilitySpec.Ability));
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
				FoundCount++;
			}
		}
		
		UE_LOG(LogRPG, Display, TEXT("AbilityInputTagPressed: Found %d matching abilities, InputPressedSpecHandles.Num = %d"), FoundCount, InputPressedSpecHandles.Num());
	}
	else
	{
		UE_LOG(LogRPG, Warning, TEXT("AbilityInputTagPressed: Invalid InputTag!"));
	}
}

void URPGAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	UE_LOG(LogRPG, Display, TEXT("AbilityInputTagReleased: InputTag = [%s]"), *InputTag.ToString());
	
	if (InputTag.IsValid())
	{
		UE_LOG(LogRPG, Display, TEXT("AbilityInputTagReleased: Searching %d abilities..."), ActivatableAbilities.Items.Num());
		
		int32 FoundCount = 0;
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				UE_LOG(LogRPG, Display, TEXT("AbilityInputTagReleased: Found matching ability [%s]"), *GetNameSafe(AbilitySpec.Ability));
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
				FoundCount++;
			}
		}
		
		UE_LOG(LogRPG, Display, TEXT("AbilityInputTagReleased: Found %d matching abilities, InputReleasedSpecHandles.Num = %d"), FoundCount, InputReleasedSpecHandles.Num());
	}
	else
	{
		UE_LOG(LogRPG, Warning, TEXT("AbilityInputTagReleased: Invalid InputTag!"));
	}
}

// ============================================================================
// ProcessAbilityInput — 每帧输入处理核心
//
// 由 PlayerController::PostProcessInput 每帧调用。
// 处理顺序很重要：
//   1. 先检查输入阻断标签（如眩晕、过场），有则清空所有输入
//   2. 处理 Held 输入 → WhileInputActive 策略的激活
//   3. 处理 Pressed 输入 → OnInputTriggered 策略的激活 + 已激活能力的 InputPressed 事件
//   4. 批量 TryActivate（避免 Held 和 Pressed 同时触发同一能力）
//   5. 处理 Released 输入 → 已激活能力的 InputReleased 事件
// ============================================================================
void URPGAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	// ── 1. 检查输入阻断 ──
	// 如果 ASC 拥有 "Gameplay.AbilityInputBlocked" 标签（如眩晕、过场动画），
	// 则清空所有输入缓存并跳过处理
	if (HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked))
	{
		ClearAbilityInput();
		return;
	}

	// 使用静态数组避免每帧堆分配
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// ── 2. 处理持续按住的输入 (WhileInputActive) ──
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const URPGGameplayAbility* RPGAbilityCDO = Cast<URPGGameplayAbility>(AbilitySpec->Ability);
				if (RPGAbilityCDO && RPGAbilityCDO->GetActivationPolicy() == ERPGAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	// ── 3. 处理按下的输入 (OnInputTriggered + InputPressed 事件) ──
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					// 能力已激活，传递 InputPressed 事件（如连击、蓄力等）
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					// 能力未激活，如果策略是 OnInputTriggered，则尝试激活
					const URPGGameplayAbility* RPGAbilityCDO = Cast<URPGGameplayAbility>(AbilitySpec->Ability);

					if (RPGAbilityCDO && RPGAbilityCDO->GetActivationPolicy() == ERPGAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	// ── 4. 批量激活 ──
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	// ── 5. 处理释放的输入 (InputReleased 事件) ──
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					// 能力已激活，传递 InputReleased 事件（如松开蓄力攻击释放）
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	// ── 6. 清空本帧缓存 ──
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

// ============================================================================
// ClearAbilityInput — 清空所有输入缓存
// ============================================================================
void URPGAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

// ============================================================================
// 能力通知回调
// ============================================================================

void URPGAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
{
	Super::NotifyAbilityActivated(Handle, Ability);

	// 能力激活时，将其添加到激活组
	// AddAbilityToActivationGroup 内部会自动取消低优先级组的能力
	if (URPGGameplayAbility* RPGAbility = Cast<URPGGameplayAbility>(Ability))
	{
		AddAbilityToActivationGroup(RPGAbility->GetActivationGroup(), RPGAbility);
	}
}

void URPGAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	Super::NotifyAbilityFailed(Handle, Ability, FailureReason);

	// 对于非本地控制的 Pawn，服务端需要通过 RPC 将失败通知转发到客户端
	if (APawn* Avatar = Cast<APawn>(GetAvatarActor()))
	{
		if (!Avatar->IsLocallyControlled() && Ability->IsSupportedForNetworking())
		{
			ClientNotifyAbilityFailed(Ability, FailureReason);
			return;
		}
	}

	// 本地控制则直接处理
	HandleAbilityFailed(Ability, FailureReason);
}

void URPGAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);

	// 能力结束时，从激活组中移除
	if (URPGGameplayAbility* RPGAbility = Cast<URPGGameplayAbility>(Ability))
	{
		RemoveAbilityFromActivationGroup(RPGAbility->GetActivationGroup(), RPGAbility);
	}
}

// ============================================================================
// ApplyAbilityBlockAndCancelTags — Tag 关系映射的核心入口
//
// 当能力激活时，GAS 内部调用此函数来处理 Block/Cancel 标签。
// 我们在此扩展：如果设置了 TagRelationshipMapping，则查表添加额外的 Block/Cancel 标签。
//
// 举例：能力 A 有 Tag "Ability.Attack"，映射表中配置了 Attack → Cancel "Ability.Move"
//       原始的 BlockTags 可能为空，但扩展后会自动添加 Cancel "Ability.Move"
// ============================================================================
void URPGAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
{
	FGameplayTagContainer ModifiedBlockTags = BlockTags;
	FGameplayTagContainer ModifiedCancelTags = CancelTags;

	if (TagRelationshipMapping)
	{
		// 查映射表，扩展 Block/Cancel 标签范围
		TagRelationshipMapping->GetAbilityTagsToBlockAndCancel(AbilityTags, &ModifiedBlockTags, &ModifiedCancelTags);
	}

	Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, ModifiedBlockTags, bExecuteCancelTags, ModifiedCancelTags);
}

void URPGAbilitySystemComponent::HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled)
{
	Super::HandleChangeAbilityCanBeCanceled(AbilityTags, RequestingAbility, bCanBeCanceled);
}

// ============================================================================
// GetAdditionalActivationTagRequirements — 查映射表获取额外激活约束
//
// 由 GAS 内部在 DoesAbilitySatisfyTagRequirements 中调用。
// 如果映射表中配置了某标签的 ActivationRequiredTags / ActivationBlockedTags，
// 则自动添加到能力的激活条件中。
//
// 举例：映射表中 "Ability.Attack" → ActivationRequired = "State.HasWeapon"
//       则攻击能力隐式要求角色必须有 "State.HasWeapon" 标签才能激活
// ============================================================================
void URPGAbilitySystemComponent::GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const
{
	if (TagRelationshipMapping)
	{
		TagRelationshipMapping->GetRequiredAndBlockedActivationTags(AbilityTags, &OutActivationRequired, &OutActivationBlocked);
	}
}

// ============================================================================
// SetTagRelationshipMapping — 设置/清除 Tag 关系映射表
// ============================================================================
void URPGAbilitySystemComponent::SetTagRelationshipMapping(URPGAbilityTagRelationshipMapping* NewMapping)
{
	TagRelationshipMapping = NewMapping;
}

// ============================================================================
// 客户端 RPC 实现 + HandleAbilityFailed
// ============================================================================
void URPGAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	HandleAbilityFailed(Ability, FailureReason);
}

void URPGAbilitySystemComponent::HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	// 将失败通知转发给 RPGGameplayAbility，由其调用 Native + Script 回调
	if (const URPGGameplayAbility* RPGAbility = Cast<const URPGGameplayAbility>(Ability))
	{
		RPGAbility->OnAbilityFailedToActivate(FailureReason);
	}	
}

// ============================================================================
// 激活组管理实现
// ============================================================================

/**
 * IsActivationGroupBlocked — 判断指定激活组是否被阻断
 *
 * 规则：
 *   Independent:       永远不被阻断
 *   Exclusive_Replaceable / Exclusive_Blocking: 
 *     当有 Exclusive_Blocking 能力正在运行时被阻断
 */
bool URPGAbilitySystemComponent::IsActivationGroupBlocked(ERPGAbilityActivationGroup Group) const
{
	bool bBlocked = false;

	switch (Group)
	{
	case ERPGAbilityActivationGroup::Independent:
		// Independent 能力永远不被阻断
		bBlocked = false;
		break;

	case ERPGAbilityActivationGroup::Exclusive_Replaceable:
	case ERPGAbilityActivationGroup::Exclusive_Blocking:
		// 只要有 Blocking 能力在运行，所有 Exclusive 能力都被阻断
		bBlocked = (ActivationGroupCounts[(uint8)ERPGAbilityActivationGroup::Exclusive_Blocking] > 0);
		break;

	default:
		checkf(false, TEXT("IsActivationGroupBlocked: Invalid ActivationGroup [%d]\n"), (uint8)Group);
		break;
	}

	return bBlocked;
}

/**
 * AddAbilityToActivationGroup — 将能力添加到激活组
 *
 * 关键逻辑：
 *   1. 增加组计数
 *   2. 如果是 Exclusive 组，自动取消所有 Replaceable 组的能力
 *   3. 确保同时最多只有 1 个 Exclusive 能力运行
 */
void URPGAbilitySystemComponent::AddAbilityToActivationGroup(ERPGAbilityActivationGroup Group, URPGGameplayAbility* RPGAbility)
{
	check(RPGAbility);
	check(ActivationGroupCounts[(uint8)Group] < INT32_MAX);

	ActivationGroupCounts[(uint8)Group]++;

	const bool bReplicateCancelAbility = false;

	switch (Group)
	{
	case ERPGAbilityActivationGroup::Independent:
		// Independent 不取消任何其他能力
		break;

	case ERPGAbilityActivationGroup::Exclusive_Replaceable:
	case ERPGAbilityActivationGroup::Exclusive_Blocking:
		// 新的 Exclusive 能力激活时，取消所有 Replaceable 组的能力
		// （如攻击激活时取消移动）
		CancelActivationGroupAbilities(ERPGAbilityActivationGroup::Exclusive_Replaceable, RPGAbility, bReplicateCancelAbility);
		break;

	default:
		checkf(false, TEXT("AddAbilityToActivationGroup: Invalid ActivationGroup [%d]\n"), (uint8)Group);
		break;
	}

	// 安全检查：同一时间最多 1 个 Exclusive 能力
	const int32 ExclusiveCount = ActivationGroupCounts[(uint8)ERPGAbilityActivationGroup::Exclusive_Replaceable] + ActivationGroupCounts[(uint8)ERPGAbilityActivationGroup::Exclusive_Blocking];
	if (!ensure(ExclusiveCount <= 1))
	{
		UE_LOG(LogRPGAbilitySystem, Error, TEXT("AddAbilityToActivationGroup: Multiple exclusive abilities are running."));
	}
}

/** 从激活组中移除能力（能力结束时调用），减少组计数 */
void URPGAbilitySystemComponent::RemoveAbilityFromActivationGroup(ERPGAbilityActivationGroup Group, URPGGameplayAbility* RPGAbility)
{
	check(RPGAbility);
	check(ActivationGroupCounts[(uint8)Group] > 0);

	ActivationGroupCounts[(uint8)Group]--;
}

/** 取消指定激活组的所有能力（排除 IgnoreAbility） */
void URPGAbilitySystemComponent::CancelActivationGroupAbilities(ERPGAbilityActivationGroup Group, URPGGameplayAbility* IgnoreRPGAbility, bool bReplicateCancelAbility)
{
	auto ShouldCancelFunc = [this, Group, IgnoreRPGAbility](const URPGGameplayAbility* RPGAbility, FGameplayAbilitySpecHandle Handle)
	{
		return ((RPGAbility->GetActivationGroup() == Group) && (RPGAbility != IgnoreRPGAbility));
	};

	CancelAbilitiesByFunc(ShouldCancelFunc, bReplicateCancelAbility);
}

// ============================================================================
// 动态标签 GameplayEffect
// ============================================================================

/**
 * AddDynamicTagGameplayEffect — 通过 GE 临时授予标签
 *
 * 实现原理：
 *   1. 获取 DynamicTagGameplayEffect 配置的 GE 类（一个 Duration=Infinite、无 Modifier 的 GE）
 *   2. 创建 GE Spec，在 DynamicGrantedTags 中添加指定标签
 *   3. 应用到自身
 *
 * 适用场景：临时状态标记（无敌、霸体、不可选中 等）
 *
 * 注意：当前使用 UPROPERTY 配置的 GE 类，后续接入 AssetManager 后可改为自动查找。
 */
void URPGAbilitySystemComponent::AddDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
	if (!DynamicTagGameplayEffect)
	{
		UE_LOG(LogRPGAbilitySystem, Warning, TEXT("AddDynamicTagGameplayEffect: DynamicTagGameplayEffect is not configured in the ASC defaults."));
		return;
	}

	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(DynamicTagGameplayEffect, 1.0f, MakeEffectContext());
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

	if (!Spec)
	{
		UE_LOG(LogRPGAbilitySystem, Warning, TEXT("AddDynamicTagGameplayEffect: Unable to make outgoing spec for [%s]."), *GetNameSafe(DynamicTagGameplayEffect));
		return;
	}

	// 在 GE Spec 的动态授予标签中添加指定标签
	Spec->DynamicGrantedTags.AddTag(Tag);

	ApplyGameplayEffectSpecToSelf(*Spec);
}

/**
 * RemoveDynamicTagGameplayEffect — 移除通过 AddDynamicTagGameplayEffect 添加的 GE
 *
 * 实现原理：
 *   查找所有拥有指定标签的 DynamicTagGameplayEffect 实例并移除
 */
void URPGAbilitySystemComponent::RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
	if (!DynamicTagGameplayEffect)
	{
		UE_LOG(LogRPGAbilitySystem, Warning, TEXT("RemoveDynamicTagGameplayEffect: DynamicTagGameplayEffect is not configured in the ASC defaults."));
		return;
	}

	// 查询条件：匹配拥有指定标签的 DynamicTagGameplayEffect 实例
	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(Tag));
	Query.EffectDefinition = DynamicTagGameplayEffect;

	RemoveActiveEffects(Query);
}

// ============================================================================
// GetAbilityTargetData — 获取能力的 TargetData
// ============================================================================
void URPGAbilitySystemComponent::GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle)
{
	TSharedPtr<FAbilityReplicatedDataCache> ReplicatedData = AbilityTargetDataMap.Find(FGameplayAbilitySpecHandleAndPredictionKey(AbilityHandle, ActivationInfo.GetActivationPredictionKey()));
	if (ReplicatedData.IsValid())
	{
		OutTargetDataHandle = ReplicatedData->TargetData;
	}
}
