// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGGameplayTags.h"

#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"
#include "RPGLogChannels.h"

namespace RPGGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_IsDead, "Ability.ActivateFail.IsDead", "Ability failed to activate because its owner is dead.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Cooldown, "Ability.ActivateFail.Cooldown", "Ability failed to activate because it is on cool down.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Cost, "Ability.ActivateFail.Cost", "Ability failed to activate because it did not pass the cost checks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_TagsBlocked, "Ability.ActivateFail.TagsBlocked", "Ability failed to activate because tags are blocking it.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_TagsMissing, "Ability.ActivateFail.TagsMissing", "Ability failed to activate because tags are missing.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Networking, "Ability.ActivateFail.Networking", "Ability failed to activate because it did not pass the network checks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_ActivationGroup, "Ability.ActivateFail.ActivationGroup", "Ability failed to activate because of its activation group.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Behavior_SurvivesDeath, "Ability.Behavior.SurvivesDeath", "An ability with this type tag should not be canceled due to death.");

	// === A2 锁定：14 个 RPG InputAction 对应的 InputTag ===
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Move,                  "InputTag.Move",                  "Move input (Arrow keys / WASD).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Native_Pickup,         "InputTag.Native.Pickup",         "Pickup / interact (F).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Native_SwitchWeapon,   "InputTag.Native.SwitchWeapon",   "Switch main/off-hand weapon (Tab).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Native_OpenMenu,       "InputTag.Native.OpenMenu",       "Open main menu / pause (Esc).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Native_OpenInventory,  "InputTag.Native.OpenInventory",  "Open inventory (I).");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability_Attack,        "InputTag.Ability.Attack",        "Main-hand basic attack (Mouse Left / A).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability_Q,             "InputTag.Ability.Q",             "Main-hand skill 1 (Q).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability_W,             "InputTag.Ability.W",             "Main-hand skill 2 (W).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability_E,             "InputTag.Ability.E",             "Off-hand skill 3 (E).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability_R,             "InputTag.Ability.R",             "Off-hand skill 4 (R).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability_Ultimate,      "InputTag.Ability.Ultimate",      "Current main-hand ultimate (Space).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability_Dodge,         "InputTag.Ability.Dodge",         "Dodge / dash (Shift).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability_Block,         "InputTag.Ability.Block",         "Block / parry (Ctrl).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability_HealItem,      "InputTag.Ability.HealItem",      "Use healing item (H).");

	// === Lyra 残留：保留以兼容 HeroComponent 的 Bind 逻辑（A4 切俯视角后再决定是否清理）===
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Look_Mouse,             "InputTag.Look.Mouse",             "[Legacy] Look (mouse) input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Look_Stick,             "InputTag.Look.Stick",             "[Legacy] Look (stick) input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Crouch,                 "InputTag.Crouch",                 "[Legacy] Crouch input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_AutoRun,                "InputTag.AutoRun",                "[Legacy] Auto-run input.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_Spawned, "InitState.Spawned", "1: Actor/component has initially spawned and can be extended");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataAvailable, "InitState.DataAvailable", "2: All required data has been loaded/replicated and is ready for initialization");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataInitialized, "InitState.DataInitialized", "3: The available data has been initialized for this actor/component, but it is not ready for full gameplay");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_GameplayReady, "InitState.GameplayReady", "4: The actor/component is fully ready for active gameplay");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Death, "GameplayEvent.Death", "Event that fires on death. This event only fires on the server.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Reset, "GameplayEvent.Reset", "Event that fires once a player reset is executed.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_RequestReset, "GameplayEvent.RequestReset", "Event to request a player's pawn to be instantly replaced with a new one at a valid spawn location.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage, "SetByCaller.Damage", "SetByCaller tag used by damage gameplay effects.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Heal, "SetByCaller.Heal", "SetByCaller tag used by healing gameplay effects.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cheat_GodMode, "Cheat.GodMode", "GodMode cheat is active on the owner.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cheat_UnlimitedHealth, "Cheat.UnlimitedHealth", "UnlimitedHealth cheat is active on the owner.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Crouching, "Status.Crouching", "Target is crouching.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_AutoRunning, "Status.AutoRunning", "Target is auto-running.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death, "Status.Death", "Target has the death status.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death_Dying, "Status.Death.Dying", "Target has begun the death process.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death_Dead, "Status.Death.Dead", "Target has finished the death process.");
						  
	// These are mapped to the movement modes inside GetMovementModeTagMap()
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Walking, "Movement.Mode.Walking", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_NavWalking, "Movement.Mode.NavWalking", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Falling, "Movement.Mode.Falling", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Swimming, "Movement.Mode.Swimming", "Default Character movement tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Flying, "Movement.Mode.Flying", "Default Character movement tag");

	// When extending Lyra, you can create your own movement modes but you need to update GetCustomMovementModeTagMap()
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Custom, "Movement.Mode.Custom", "This is invalid and should be replaced with custom tags.  See LyraGameplayTags::CustomMovementModeTagMap.");

	// =====================================================================================
	// === A3 v7 锁定：RPG GAS 属性 / SetByCaller / 战斗状态 / 消息 Tag 定义 ===
	// =====================================================================================

	// --- HealthSet 6 字段 ---
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Health_Final,            "RPG.Attribute.Health.Final",          "[A3] HealthSet.HealthFinal current HP value.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Health_Max,              "RPG.Attribute.Health.Max",            "[A3] HealthSet.HealthMax derived from PrimaryAttributes.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Health_Healing,          "RPG.Attribute.Health.Healing",        "[A3] HealthSet.HealthHealing meta channel.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Health_Damage,           "RPG.Attribute.Health.Damage",         "[A3] HealthSet.HealthDamage meta channel.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Stamina_Current,         "RPG.Attribute.Stamina.Current",       "[A3] HealthSet.StaminaCurrent (block durability).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Stamina_Max,             "RPG.Attribute.Stamina.Max",           "[A3] HealthSet.StaminaMax.");

	// --- PrimaryAttributeSet 21 字段 ---
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Health_Base,             "RPG.Attribute.Health.Base",           "[A3] PrimaryAttributeSet.HealthBase (legacy: Hp_Basic).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Attack_Base,             "RPG.Attribute.Attack.Base",           "[A3] PrimaryAttributeSet.AttackBase (legacy: Atk_Basic).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Defense_Base,            "RPG.Attribute.Defense.Base",          "[A3] PrimaryAttributeSet.DefenseBase (legacy: Def_Basic).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Health_Bonus,            "RPG.Attribute.Health.Bonus",          "[A3] PrimaryAttributeSet.HealthBonus (legacy: Hp_PostAdd).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Attack_Bonus,            "RPG.Attribute.Attack.Bonus",          "[A3] PrimaryAttributeSet.AttackBonus (legacy: Atk_PostAdd).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Defense_Bonus,           "RPG.Attribute.Defense.Bonus",         "[A3] PrimaryAttributeSet.DefenseBonus (legacy: Def_PostAdd).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Health_Mul,              "RPG.Attribute.Health.Mul",            "[A3] PrimaryAttributeSet.HealthMul (legacy: Hp_Mul).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Attack_Mul,              "RPG.Attribute.Attack.Mul",            "[A3] PrimaryAttributeSet.AttackMul (legacy: Atk_Mul).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Defense_Mul,             "RPG.Attribute.Defense.Mul",           "[A3] PrimaryAttributeSet.DefenseMul (legacy: Def_Mul).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Attack_Final,            "RPG.Attribute.Attack.Final",          "[A3] PrimaryAttributeSet.AttackFinal (derived).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Defense_Final,           "RPG.Attribute.Defense.Final",         "[A3] PrimaryAttributeSet.DefenseFinal (derived).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Crit_Chance,             "RPG.Attribute.Crit.Chance",           "[A3] Crit chance probability (legacy: Crit).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Crit_Damage,             "RPG.Attribute.Crit.Damage",           "[A3] Crit damage multiplier (legacy: CritDmg).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Defense_Penetration,     "RPG.Attribute.Defense.Penetration",   "[A3] Defense penetration ratio (legacy: IgnDef).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Damage_Bonus,            "RPG.Attribute.Damage.Bonus",          "[A3] Damage output bonus (legacy: Dmg_Mul).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Damage_Reduction,        "RPG.Attribute.Damage.Reduction",      "[A3] Damage taken reduction (legacy: DmgRed_Mul).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_LifeSteal,               "RPG.Attribute.LifeSteal",             "[A3] Life steal ratio (legacy: HealthSteal).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_EnergyGainMul,           "RPG.Attribute.EnergyGainMul",         "[A3] Energy gain multiplier (legacy: PowerRegenRate, see 02 §F.9).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_BreakBonus,              "RPG.Attribute.BreakBonus",            "[A3] Break (toughness) bonus.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_AttackSpeed,             "RPG.Attribute.AttackSpeed",           "[A3] Attack speed bonus (legacy: NormalSkillSpeed).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MoveSpeed,               "RPG.Attribute.MoveSpeed",             "[A3] Move speed (UE units/s).");

	// --- SetByCaller ---
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage_Base,                  "RPG.SetByCaller.Damage.Base",                  "[A3] Base damage value passed via GE Spec.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage_AttackerScalingCoef,   "RPG.SetByCaller.Damage.AttackerScalingCoef",   "[A3] Coefficient applied to attacker's AttackFinal in damage formula.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Block_DamageReduction,        "RPG.SetByCaller.Block.DamageReduction",        "[A3] Block damage reduction (0..1) carried by GE_Block_State.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Block_StaminaConsume,         "RPG.SetByCaller.Block.StaminaConsume",         "[A3] Stamina cost ratio when blocking (cost = damage * this).");

	// --- State ---
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Combat_Active,                "RPG.State.Combat.Active",              "[A3] Owner is in combat (set by URPGCombatStateSubsystem).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Block_Blocking,               "RPG.State.Block.Blocking",             "[A3] Owner is currently blocking (granted by GE_Block_State).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Block_PerfectBlocking,        "RPG.State.Block.PerfectBlocking",      "[A3] Owner is within perfect block window (set by Block GA timer).");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Block_Broken,                 "RPG.State.Block.Broken",               "[A3] Owner's stamina ran out, block broken stun active.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Stamina_RegenOutOfCombat,     "RPG.State.Stamina.RegenOutOfCombat",   "[A3] Granted by GE_Stamina_Regen_OutOfCombat.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Stamina_RegenInCombat,        "RPG.State.Stamina.RegenInCombat",      "[A3] Granted by GE_Stamina_Regen_InCombat.");

	// --- Message ---
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_Damage_Popup,               "RPG.Message.Damage.Popup",             "[A3] Broadcast by DamageExecution to drive damage number popup.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_Block_PerfectTriggered,     "RPG.Message.Block.PerfectTriggered",   "[A3] Broadcast when target perfectly blocks an attack.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_Block_Broken,               "RPG.Message.Block.Broken",             "[A3] Broadcast when StaminaCurrent reaches 0 during blocking.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_Health_OutOfHealth,         "RPG.Message.Health.OutOfHealth",       "[A3] Broadcast when HealthFinal reaches 0.");

	// Unreal Movement Modes
	const TMap<uint8, FGameplayTag> MovementModeTagMap =
	{
		{ MOVE_Walking, Movement_Mode_Walking },
		{ MOVE_NavWalking, Movement_Mode_NavWalking },
		{ MOVE_Falling, Movement_Mode_Falling },
		{ MOVE_Swimming, Movement_Mode_Swimming },
		{ MOVE_Flying, Movement_Mode_Flying },
		{ MOVE_Custom, Movement_Mode_Custom }
	};

	// Custom Movement Modes
	const TMap<uint8, FGameplayTag> CustomMovementModeTagMap =
	{
		// Fill these in with your custom modes
	};

	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
	{
		const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

		if (!Tag.IsValid() && bMatchPartialString)
		{
			FGameplayTagContainer AllTags;
			Manager.RequestAllGameplayTags(AllTags, true);

			for (const FGameplayTag& TestTag : AllTags)
			{
				if (TestTag.ToString().Contains(TagString))
				{
					UE_LOG(LogLyra, Display, TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."), *TagString, *TestTag.ToString());
					Tag = TestTag;
					break;
				}
			}
		}

		return Tag;
	}
}

