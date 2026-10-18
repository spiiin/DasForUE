using UnrealBuildTool;
public class DasStarterEditorTarget : TargetRules {
 public DasStarterEditorTarget(TargetInfo Target):base(Target){Type=TargetType.Editor;DefaultBuildSettings=BuildSettingsVersion.V7;IncludeOrderVersion=EngineIncludeOrderVersion.Unreal5_8;ExtraModuleNames.AddRange(new[]{"DasStarter","DasStarterEditor"});}
}
