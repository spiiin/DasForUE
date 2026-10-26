using UnrealBuildTool;
public class CleanGameplayEditor : ModuleRules {
    public CleanGameplayEditor(ReadOnlyTargetRules Target) : base(Target) {
        PrivateDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "UnrealEd", "DasRuntime", "DasEditor" });
    }
}
