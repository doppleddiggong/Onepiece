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
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Voice",     
			"AudioMixer" 
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"AudioCapture",
			"AudioCaptureCore",
			"AudioPlatformConfiguration",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"MediaAssets",
			"Media"
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoffeeLibrary",
			"LatteLibrary",
			"HyperLinkPlugin"
		});
		
		PublicIncludePaths.AddRange(new string[] {
			Path.Combine(ModuleDirectory, "Character", "Public"),
			Path.Combine(ModuleDirectory, "Game", "Public"),
			Path.Combine(ModuleDirectory, "Interactable", "Public"),
			Path.Combine(ModuleDirectory, "MessageBox", "Public"),
			Path.Combine(ModuleDirectory, "Network", "Public"),
			Path.Combine(ModuleDirectory, "Prop", "Public"),
			Path.Combine(ModuleDirectory, "UI", "Public"),
			Path.Combine(ModuleDirectory, "Voice", "Public"),
			Path.Combine(ModuleDirectory, "WidgetComponents", "Public"),
			Path.Combine(ModuleDirectory, "ThirdParty", "Public"),
		});
	}
}
