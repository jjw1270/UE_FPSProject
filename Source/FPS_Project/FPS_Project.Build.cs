// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class FPS_Project : ModuleRules
{
	public FPS_Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Sockets", "Networking" });

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        PublicIncludePaths.AddRange(
            new string[] {
                ModuleDirectory,
                Path.Combine(ModuleDirectory, "Public"),
                Path.Combine(ModuleDirectory, "Public/Login"),
            }
        );
    }
}
