using UnrealBuildTool;
public class DasForUESmoke : ModuleRules
{
    public DasForUESmoke(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "DasRuntime", "EnhancedInput", "UMG", "AnimGraphRuntime", "Niagara", "AIModule", "NavigationSystem" });
    }
}
