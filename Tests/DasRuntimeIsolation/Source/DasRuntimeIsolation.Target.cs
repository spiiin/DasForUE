using UnrealBuildTool;
public class DasRuntimeIsolationTarget : TargetRules
{
    public DasRuntimeIsolationTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("DasRuntimeIsolation");
    }
}
