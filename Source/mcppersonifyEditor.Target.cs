using UnrealBuildTool;

public class mcppersonifyEditorTarget : TargetRules
{
	public mcppersonifyEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
		ExtraModuleNames.Add("mcppersonify");
	}
}
