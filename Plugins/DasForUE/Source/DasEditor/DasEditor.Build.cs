using UnrealBuildTool;
public class DasEditor : ModuleRules
{
    public DasEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "DasRuntime", "UnrealEd", "Kismet", "KismetCompiler", "BlueprintGraph" });
    }
}
