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
			"CommonInput",
			"CommonGame",
			"CommonUser",
			"UIExtension",
			"GameplayMessageRuntime",
			"MotionWarping",
			"LevelSequence" ,
			"MovieScene"
		});
		

		PublicIncludePaths.AddRange(new string[] {
			"MyCPPProject"
		});

		//Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
