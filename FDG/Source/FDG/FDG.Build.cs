// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FDG : ModuleRules
{
	public FDG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayAbilities", "GameplayTags", "GameplayTasks", "DeveloperSettings", "UMG", "Slate", "SlateCore", "GameFeatures", "ModularGameplay", "ModularGameplayActors", "CommonUI", "CommonUser", "CommonGame", "CommonLoadingScreen", "GameplayMessageRuntime", "UIExtension", "AsyncMixin", "GameSettings", "GameSubtitles", "ControlFlows", "DataRegistry", "AnimationWarpingRuntime", "Niagara" });

		// RPGGAME_API 全局映射：所有从 Lyra 提取的 Skill 代码使用 RPGGAME_API，
		// 通过编译器定义映射到本项目的 FDG_API，无需每个文件手动 include
		PublicDefinitions.Add("RPGGAME_API=FDG_API");

		PublicIncludePaths.Add(ModuleDirectory);

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
