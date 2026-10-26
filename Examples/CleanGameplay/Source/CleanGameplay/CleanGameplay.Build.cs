using UnrealBuildTool;
public class CleanGameplay : ModuleRules {
    public CleanGameplay(ReadOnlyTargetRules Target) : base(Target) {
        PrivateDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "InputCore", "DasRuntime" });
    }
}
