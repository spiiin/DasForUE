using UnrealBuildTool;
public class DasForUESmokeTarget : TargetRules
{
    public DasForUESmokeTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("DasForUESmoke");
    }
}
