using UnrealBuildTool;
public class DasEditor : ModuleRules
{
    public DasEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "DasRuntime" });
        PrivateDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "DasRuntime", "Projects", "UnrealEd", "Json", "Kismet", "KismetCompiler", "BlueprintGraph", "AssetRegistry", "EnhancedInput", "InputCore", "SlateCore", "Slate", "SlateNullRenderer", "ApplicationCore", "UMG", "MovieScene", "UMGEditor", "Niagara", "AIModule", "NavigationSystem", "PhysicsCore", "Chaos" });
    }
}
