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
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "GameplayTags", "UMG", "MovieScene", "AIModule" });
        PrivateDependencyModuleNames.AddRange(new[] { "Projects", "Json", "AssetRegistry", "InputCore", "EnhancedInput", "Slate", "SlateCore", "Niagara", "NavigationSystem", "PhysicsCore", "Chaos" });
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
        // Enumerate dependencies in the rules so the receipt contains every staged script.
        string ProjectRoot = Target.ProjectFile == null ? "" : Target.ProjectFile.Directory.FullName;
        string GeneratedRoot = Path.Combine(ProjectRoot, "Source", "DasGenerated");
        string NativeHeader = Path.Combine(GeneratedRoot, "DasProjectNativeGameplay.generated.h");
        bool ProjectNative = ProjectRoot.Length != 0 && File.Exists(NativeHeader);
        PrivateDefinitions.Add("DAS_PROJECT_NATIVE_GAMEPLAY=" + (ProjectNative ? "1" : "0"));
        if (ProjectNative) { PrivateIncludePaths.Add(GeneratedRoot); ExternalDependencies.Add(NativeHeader); }
        string ProjectScripts = Path.Combine(ProjectRoot, "Content", "Das", "Scripts");
        if (ProjectRoot.Length != 0 && Directory.Exists(ProjectScripts))
            foreach (string File in Directory.GetFiles(ProjectScripts, "*", SearchOption.AllDirectories))
            { RuntimeDependencies.Add(File, StagedFileType.UFS); ExternalDependencies.Add(File); }
        string Scripts = Path.Combine(PluginDirectory, "Content", "Das");
        foreach (string File in Directory.GetFiles(Scripts, "*", SearchOption.AllDirectories))
        {
            RuntimeDependencies.Add(File, StagedFileType.UFS);
            ExternalDependencies.Add(File);
        }
    }
}
