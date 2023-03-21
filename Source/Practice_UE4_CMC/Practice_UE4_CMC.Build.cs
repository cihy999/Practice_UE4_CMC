// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Practice_UE4_CMC : ModuleRules
{
	public Practice_UE4_CMC(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
