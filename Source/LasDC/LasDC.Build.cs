// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class LasDC : ModuleRules
{
	public LasDC(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "NetCore", "Engine",
			"LasDC", "InputCore", "GameplayAbilities", "GameplayTags", 
			"GameplayTasks", "HeadMountedDisplay", "Niagara",
			"EnhancedInput", "MotionWarping", "UMG", "Slate", "SlateCore"});


        PublicIncludePaths.AddRange(new string[] { 
			Path.Combine(ModuleDirectory, "Public/Character"), 
			Path.Combine(ModuleDirectory, "Public/DataAssets")
		
		});
    }
}
