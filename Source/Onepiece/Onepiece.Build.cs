// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Onepiece : ModuleRules
{
	public Onepiece(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"EnhancedInput", 
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"InputCore",
			"UMG",
			"Slate",
			"SlateCore",
		});
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"HTTP",
			"Json",
			"JsonUtilities",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"WebSockets",
		});
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"OnlineSubsystem",
			"OnlineSubsystemSteam",
		});
		
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"DeveloperSettings"
		});
		
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoffeeLibrary",
			"LatteLibrary"
		});
		
		PublicIncludePaths.AddRange(new string[] {
			Path.Combine(ModuleDirectory, "Character", "Public"),
			Path.Combine(ModuleDirectory, "Network", "Public"),
			Path.Combine(ModuleDirectory, "UI", "Public"),
		});
	}
}
