// Copyright RPG Demo Project. Derived from CommonGame.

#include "RPGUIPolicy.h"

#include "GameUIManagerSubsystem.h"
#include "Engine/GameInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGUIPolicy)

UWorld* URPGUIPolicy::GetWorld() const
{
	// CDO / 蓝图编辑器中：Outer 是 Package（不是 Subsystem），原版 CastChecked 会 Fatal。
	// 这里在编辑器 CDO/Archetype 阶段安全返回 nullptr，让 PropertyEditor 的 Transaction 流程不会崩。
	if (HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
	{
		return nullptr;
	}

	UObject* Outer = GetOuter();
	if (!Outer)
	{
		return nullptr;
	}

	UGameUIManagerSubsystem* OwningUIManager = Cast<UGameUIManagerSubsystem>(Outer);
	if (!OwningUIManager)
	{
		// Editor 里通过 Reparent / Duplicate 等场景下 Outer 可能既不是 Package 也不是 Subsystem，
		// 走 Outer 链兜底（GameInstance → World）。
		return Outer->GetWorld();
	}

	UGameInstance* GameInstance = OwningUIManager->GetGameInstance();
	return GameInstance ? GameInstance->GetWorld() : nullptr;
}
