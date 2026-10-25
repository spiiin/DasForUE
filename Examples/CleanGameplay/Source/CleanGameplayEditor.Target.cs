using UnrealBuildTool;
public class CleanGameplayEditorTarget : TargetRules {
    public CleanGameplayEditorTarget(TargetInfo Target) : base(Target) {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("CleanGameplay");
        ExtraModuleNames.Add("CleanGameplayEditor");
    }
}
