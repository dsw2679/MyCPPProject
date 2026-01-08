// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MyCPPProject : ModuleRules
{
	public MyCPPProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"Niagara",
			"UMG",
			"Slate",
			"ModularGameplay",
			"ModularGameplayActors",
			"GameplayTags",
			"GameplayTasks",
			"GameplayAbilities",
			"GameFeatures",
			"CommonUI",
			"CommonGame",
			"CommonUser",
			"UIExtension",
			"GameplayMessageRuntime",
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"MyCPPProject",
			"MyCPPProject/Variant_Strategy",
			"MyCPPProject/Variant_Strategy/UI",
			"MyCPPProject/Variant_TwinStick",
			"MyCPPProject/Variant_TwinStick/AI",
			"MyCPPProject/Variant_TwinStick/Gameplay",
			"MyCPPProject/Variant_TwinStick/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
