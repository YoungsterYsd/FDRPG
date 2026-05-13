// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class RPGEditorTarget : TargetRules
{
	public RPGEditorTarget(TargetInfo Target) : base(Target)
	{
		DefaultBuildSettings = BuildSettingsVersion.V6;

		Type = TargetType.Editor;
		ExtraModuleNames.AddRange(new string[] { "RPGGame", "RPGEditor" });

		if (!bBuildAllModules)
		{
			NativePointerMemberBehaviorOverride = PointerMemberBehavior.Disallow;
		}

		RPGGameTarget.ApplySharedLyraTargetSettings(this);

		// This is used for touch screen development along with the "Unreal Remote 2" app
		EnablePlugins.Add("RemoteSession");
	}
}
