// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "Input/RPGInputConfig.h"
#include "RPGInputComponent.generated.h"

class URPGInputConfig;
class URPGAbilitySystemComponent;

/**
 * URPGInputComponent
 *
 *	扩展的 EnhancedInputComponent，提供基于 GameplayTag 的输入绑定。
 *	由 HeroComponent 使用，将 InputAction 通过 Tag 映射到 Native 回调或 ASC 能力输入。
 */
UCLASS()
class RPGGAME_API URPGInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	URPGInputComponent();

	/**
	 * Bind a native input action (e.g., Move, Look) for the given tag.
	 * Native actions are not bound to GAs - they call a function directly.
	 */
	template<class UserClass, typename FuncType>
	void BindNativeAction(const URPGInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func);

	/**
	 * Bind all ability input actions from the InputConfig.
	 * Ability actions are routed through ASC's OnAbilityInputPressed/Released.
	 * Uses lambdas to capture the InputTag and forward to the callback.
	 */
	void BindAbilityActions(const URPGInputConfig* InputConfig, URPGAbilitySystemComponent* ASC, TArray<uint32>& BindHandles);

	/** Remove ability bindings by handle */
	void RemoveBinds(TArray<uint32>& BindHandles);
};

//////////////////////////////////////////////////////////////////////////
// Template implementations
//////////////////////////////////////////////////////////////////////////

template<class UserClass, typename FuncType>
void URPGInputComponent::BindNativeAction(const URPGInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
{
	check(InputConfig);

	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}
