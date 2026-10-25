using UnrealBuildTool;
public class DasRuntimeIsolationEditorTarget : TargetRules
{
    public DasRuntimeIsolationEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("DasRuntimeIsolation");
    }
}
