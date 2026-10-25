using UnrealBuildTool;
public class DasRuntimeIsolation : ModuleRules
{
    public DasRuntimeIsolation(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "DasRuntime" });
    }
}
