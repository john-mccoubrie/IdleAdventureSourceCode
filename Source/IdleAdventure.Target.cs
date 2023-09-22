

using UnrealBuildTool;
using System.Collections.Generic;

public class IdleAdventureTarget : TargetRules
{
	public IdleAdventureTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "IdleAdventure" } );
	}
}
