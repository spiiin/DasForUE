using UnrealBuildTool;
using System.IO;

public class DasRuntime : ModuleRules
{
    public DasRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        if (Target.Platform != UnrealTargetPlatform.Win64)
            throw new BuildException("DasRuntime foundation currently supports Win64 only.");
        PCHUsage = PCHUsageMode.NoPCHs;
        bUseUnity = false;
        bEnableExceptions = true;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine" });
        PrivateDependencyModuleNames.AddRange(new[] { "Projects", "Json" });
        string Root = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../.."));
        string Das = Path.Combine(Root, "ThirdParty", "daScript");
        PublicSystemIncludePaths.Add(Path.Combine(Das, "include"));
        PublicSystemIncludePaths.Add(Path.Combine(Das, "3rdparty", "fmt", "include"));
        PrivateDefinitions.AddRange(new[] {
            "DAS_ENABLE_EXCEPTIONS=1", "DAS_FREE_LIST=1", "DAS_NO_ASSERTIONS=1",
            "DAS_ENABLE_DYN_INCLUDES=1", "DAS_NO_GLOBAL_NEW_AND_DELETE=1", "DAS_FUSION=2"
        });
        foreach (string Library in new[] { "libDaScript", "libDaScript_runtime", "libUriParser" })
            PublicAdditionalLibraries.Add(Path.Combine(Das, "lib", "Release", Library + ".lib"));
        PublicSystemLibraries.AddRange(new[] { "ws2_32.lib", "psapi.lib", "bcrypt.lib" });
        RuntimeDependencies.Add("$(PluginDir)/Content/Das/...", StagedFileType.UFS);
    }
}
