using UnrealBuildTool;
using System.IO;
using EpicGames.Core;

public class DasRuntime : ModuleRules
{
    public DasRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
        if (Target.Platform != UnrealTargetPlatform.Win64)
            throw new BuildException("DasRuntime foundation currently supports Win64 only.");
        // Explicit opt-in for the build-agent Shipping probe; ordinary Shipping excludes it.
        bool ShippingProbe = Target.Configuration == UnrealTargetConfiguration.Shipping &&
            System.Environment.GetEnvironmentVariable("DAS_GAS_SHIPPING_VALIDATION") == "1";
        PrivateDefinitions.Add("DAS_GAS_SHIPPING_VALIDATION=" + (ShippingProbe ? "1" : "0"));
        // Editor commandlets need probes; game/server builds require explicit project opt-in.
        string ProbeOptIn = Target.ProjectFile == null ? null :
            Path.Combine(Target.ProjectFile.Directory.FullName, "Config", "DasIncludeTestFixtures.txt");
        bool TestProbes = Target.Type == TargetType.Editor;
        if (ProbeOptIn != null && File.Exists(ProbeOptIn))
        {
            ExternalDependencies.Add(ProbeOptIn);
            if (File.ReadAllText(ProbeOptIn).Trim() != "include-plugin-test-fixtures")
                throw new BuildException("Invalid DAS test-fixture opt-in: " + ProbeOptIn);
            TestProbes = true;
        }
        PublicDefinitions.Add("DAS_WITH_TEST_PROBES=" + (TestProbes ? "1" : "0"));
        if (TestProbes)
        {
            string Tests = Path.Combine(PluginDirectory, "Tests", "Runtime");
            if (!ConditionalAddModuleDirectory(new DirectoryReference(Tests)))
                throw new BuildException("Missing DAS probe sources: " + Tests);
            PublicIncludePaths.Add(Path.Combine(Tests, "Public"));
            PrivateIncludePaths.Add(Path.Combine(Tests, "Private"));
            PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
        }
        PCHUsage = PCHUsageMode.NoPCHs;
        bUseUnity = false;
        bEnableExceptions = true;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "GameplayTags", "GameplayAbilities", "GameplayTasks", "UMG", "MovieScene", "AIModule" });
        PrivateDependencyModuleNames.AddRange(new[] { "Projects", "Json", "AssetRegistry", "InputCore", "ApplicationCore", "EnhancedInput", "Slate", "SlateCore", "Niagara", "NavigationSystem", "PhysicsCore", "Chaos" });
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
        string Scripts = Path.Combine(PluginDirectory, "Content", "Das");
        string Extras = ProjectRoot.Length == 0 ? null : Path.Combine(ProjectRoot, "Config", "DasPluginScripts.txt");
        ExternalDependencies.Add(Path.Combine(Scripts, "RuntimeScripts.txt"));
        if (Extras != null)
        {
            if (System.IO.File.Exists(Extras)) ExternalDependencies.Add(Extras);
            string TestOptIn = Path.Combine(ProjectRoot, "Config", "DasIncludeTestFixtures.txt");
            if (System.IO.File.Exists(TestOptIn)) ExternalDependencies.Add(TestOptIn);
        }
        foreach (string File in DasScriptStaging.SelectPluginFiles(Scripts, Extras))
        {
            RuntimeDependencies.Add(File, StagedFileType.UFS);
            ExternalDependencies.Add(File);
        }
        // Project-owned scripts are intentional gameplay input; data files require explicit opt-in.
        string ProjectScripts = Path.Combine(ProjectRoot, "Content", "Das", "Scripts");
        if (ProjectRoot.Length != 0 && Directory.Exists(ProjectScripts))
            foreach (string File in Directory.GetFiles(ProjectScripts, "*.das", SearchOption.AllDirectories))
            { RuntimeDependencies.Add(File, StagedFileType.UFS); ExternalDependencies.Add(File); }
        string ProjectData = Path.Combine(ProjectRoot, "Config", "DasProjectData.txt");
        if (ProjectRoot.Length != 0)
        {
            if (System.IO.File.Exists(ProjectData)) ExternalDependencies.Add(ProjectData);
            foreach (string File in DasScriptStaging.ReadManifest(ProjectRoot, ProjectData, false))
            { RuntimeDependencies.Add(File, StagedFileType.UFS); ExternalDependencies.Add(File); }
        }
    }
}

// Kept free of UBT types so focused tests execute the actual receipt-selection implementation.
public static class DasScriptStaging
{
    public static System.Collections.Generic.IEnumerable<string> ReadManifest(string Root, string Manifest, bool Required)
    {
        if (!System.IO.File.Exists(Manifest))
        {
            if (Required) throw new System.IO.FileNotFoundException("Missing DAS staging manifest", Manifest);
            yield break;
        }
        string Prefix = Path.GetFullPath(Root).TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar)
            + Path.DirectorySeparatorChar;
        foreach (string Raw in System.IO.File.ReadAllLines(Manifest))
        {
            string Entry = Raw.Trim();
            if (Entry.Length == 0 || Entry.StartsWith("#")) continue;
            if (Path.IsPathRooted(Entry) || Entry.IndexOfAny(new[] { '*', '?' }) >= 0)
                throw new System.IO.InvalidDataException("DAS staging requires an exact relative file: " + Entry);
            string Full = Path.GetFullPath(Path.Combine(Root, Entry));
            if (!Full.StartsWith(Prefix, System.StringComparison.OrdinalIgnoreCase) || !System.IO.File.Exists(Full))
                throw new System.IO.InvalidDataException("Missing or escaping DAS staging file: " + Entry);
            yield return Full;
        }
    }

    public static string[] SelectPluginFiles(string Root, string Extras)
    {
        Root = Path.GetFullPath(Root);
        var Files = new System.Collections.Generic.SortedSet<string>(System.StringComparer.OrdinalIgnoreCase);
        foreach (string File in ReadManifest(Root, Path.Combine(Root, "RuntimeScripts.txt"), true)) Files.Add(File);
        // daslib is the compiler/runtime standard library, not our test scripts. Ignore docs and lint artefacts.
        string Library = Path.Combine(Root, "daslib");
        if (!Directory.Exists(Library)) throw new System.IO.DirectoryNotFoundException("Missing DAS standard library: " + Library);
        foreach (string File in Directory.GetFiles(Library, "*.das", SearchOption.AllDirectories)) Files.Add(File);
        if (Extras != null)
        {
            foreach (string File in ReadManifest(Root, Extras, false)) Files.Add(File);
            // Deliberate opt-in is separate from the runtime manifest and never inferred from configuration.
            string TestOptIn = Path.Combine(Path.GetDirectoryName(Extras), "DasIncludeTestFixtures.txt");
            if (System.IO.File.Exists(TestOptIn))
            {
                if (System.IO.File.ReadAllText(TestOptIn).Trim() != "include-plugin-test-fixtures")
                    throw new System.IO.InvalidDataException("Invalid DAS test-fixture opt-in: " + TestOptIn);
                foreach (string File in Directory.GetFiles(Path.Combine(Root, "Scripts"), "*", SearchOption.AllDirectories))
                    if (Path.GetExtension(File).Equals(".das", System.StringComparison.OrdinalIgnoreCase)
                        || Path.GetExtension(File).Equals(".json", System.StringComparison.OrdinalIgnoreCase)) Files.Add(File);
            }
        }
        return System.Linq.Enumerable.ToArray(Files);
    }
}
