using UnrealBuildTool;

public class mcppersonifyTarget : TargetRules
{
	public mcppersonifyTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("mcppersonify");
	}
}
