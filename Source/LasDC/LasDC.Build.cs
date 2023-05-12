// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LasDC : ModuleRules
{
	public LasDC(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "LasDC", "InputCore", "GameplayAbilities", "GameplayTags", "GameplayTasks", "HeadMountedDisplay", "EnhancedInput" });

    }
}
