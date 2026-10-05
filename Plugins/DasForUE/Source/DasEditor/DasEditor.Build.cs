using UnrealBuildTool;
public class DasEditor : ModuleRules
{
    public DasEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "DasRuntime" });
        PrivateDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "DasRuntime", "UnrealEd", "Kismet", "KismetCompiler", "BlueprintGraph" });
    }
}
