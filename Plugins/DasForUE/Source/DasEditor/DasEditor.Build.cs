using UnrealBuildTool;
using System.IO;
using EpicGames.Core;
public class DasEditor : ModuleRules
{
    public DasEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        string OptIn = Target.ProjectFile == null ? null : Path.Combine(Target.ProjectFile.Directory.FullName, "Config", "DasIncludeEditorTests.txt");
        bool EditorTests = OptIn != null && File.Exists(OptIn);
        if (EditorTests)
        {
            PrivateDependencyModuleNames.Add("SlateNullRenderer");
            ExternalDependencies.Add(OptIn);
            if (File.ReadAllText(OptIn).Trim() != "include-plugin-editor-tests")
                throw new BuildException("Invalid DAS editor-test opt-in: " + OptIn);
            string Tests = Path.Combine(PluginDirectory, "Tests", "Editor");
            if (!ConditionalAddModuleDirectory(new DirectoryReference(Tests)))
                throw new BuildException("Missing DAS editor tests: " + Tests);
            PublicIncludePaths.Add(Path.Combine(Tests, "Public"));
            PrivateIncludePaths.Add(Path.Combine(Tests, "Private"));
            PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
        }
        PrivateDefinitions.Add("DAS_WITH_EDITOR_TESTS=" + (EditorTests ? "1" : "0"));
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new[] { "Core", "DasRuntime" });
        PrivateDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "GameplayTags", "GameplayAbilities", "DasRuntime", "Projects", "UnrealEd", "Json", "Kismet", "KismetCompiler", "BlueprintGraph", "AssetRegistry", "DirectoryWatcher", "EnhancedInput", "InputCore", "SlateCore", "Slate", "ApplicationCore", "UMG", "MovieScene", "UMGEditor", "Niagara", "AIModule", "NavigationSystem", "PhysicsCore", "Chaos" });
    }
}
