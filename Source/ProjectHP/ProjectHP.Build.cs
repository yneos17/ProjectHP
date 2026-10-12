// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class ProjectHP : ModuleRules
{
	public ProjectHP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivatePCHHeaderFile = "Base/Pch.h";

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "XmlParser",
        });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        PublicIncludePaths.AddRange(
            new string[] {
                "ProjectHP",
                Path.Combine(ModuleDirectory, ""),
                Path.Combine(ModuleDirectory, "Base"),
                Path.Combine(ModuleDirectory, "GameCommon"),
                Path.Combine(ModuleDirectory, "Actor"),
                Path.Combine(ModuleDirectory, "ActorComponent"),
                Path.Combine(ModuleDirectory, "Camera"),
                Path.Combine(ModuleDirectory, "Character"),
                Path.Combine(ModuleDirectory, "Editor"),
            }
        );

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
