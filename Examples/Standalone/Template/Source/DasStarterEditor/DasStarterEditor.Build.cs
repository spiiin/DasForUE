using UnrealBuildTool;
public class DasStarterEditor:ModuleRules {
 public DasStarterEditor(ReadOnlyTargetRules Target):base(Target){PCHUsage=PCHUsageMode.UseExplicitOrSharedPCHs;PrivateDependencyModuleNames.AddRange(new[]{"Core","CoreUObject","Engine","UnrealEd","DasRuntime","DasEditor"});}
}
