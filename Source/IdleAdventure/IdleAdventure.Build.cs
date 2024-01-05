

using UnrealBuildTool;

public class IdleAdventure : ModuleRules
{
	public IdleAdventure(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayAbilities", 
            "PlayFab", "PlayFabCpp", "PlayFabCommon", "PhotonCloudAPI", "AIModule", "Niagara", "NavigationSystem", "Json", "Slate", "SlateCore", "Steamworks", "OnlineSubsystem", "OnlineSubsystemUtils" });

		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayTags", "GameplayTasks", "PhotonCloudAPI" });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        PrivateDependencyModuleNames.Add("OnlineSubsystem");
        PrivateDependencyModuleNames.Add("OnlineSubsystemSteam");
        PrivateIncludePathModuleNames.AddRange(new string[] { "PhotonCloudAPI" });
        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
