using UnrealBuildTool;
public class DasForUESmokeEditorTarget : TargetRules
{
    public DasForUESmokeEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("DasForUESmoke");
    }
}
