using UnrealBuildTool;
public class DasEditorIsolation : ModuleRules {
    public DasEditorIsolation(ReadOnlyTargetRules Target) : base(Target) {
        PrivateDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "DasRuntime", "DasEditor", "UnrealEd" });
    }
}
