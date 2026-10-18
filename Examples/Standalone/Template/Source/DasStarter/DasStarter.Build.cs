using UnrealBuildTool;
public class DasStarter:ModuleRules {
 public DasStarter(ReadOnlyTargetRules Target):base(Target){PCHUsage=PCHUsageMode.UseExplicitOrSharedPCHs;PrivateDependencyModuleNames.AddRange(new[]{"Core","CoreUObject","Engine","DasRuntime"});}
}
