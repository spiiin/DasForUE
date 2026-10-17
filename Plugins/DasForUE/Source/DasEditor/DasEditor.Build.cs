using UnrealBuildTool;
public class DasEditor : ModuleRules
{
    public DasEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "DasRuntime" });
        PrivateDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "DasRuntime", "UnrealEd", "Json", "Kismet", "KismetCompiler", "BlueprintGraph", "AssetRegistry", "EnhancedInput", "InputCore", "UMG", "UMGEditor", "Niagara", "AIModule", "NavigationSystem" });
    }
}
