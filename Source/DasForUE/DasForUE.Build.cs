// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class DasForUE : ModuleRules
{
	public DasForUE(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		//PCHUsage = ModuleRules.PCHUsageMode.NoSharedPCHs; //use for check if some headers were forgotten
		
		string PluginDir = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../"));
		PublicAdditionalLibraries.Add(Path.Combine(PluginDir, "daScript/cmake_temp/Release/libDaScript.lib"));
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
		);

		
		PrivateIncludePaths.AddRange(
			new string[] {
				"../daScript/include"
			}
		);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				
				"Projects", //IPluginManager
			}
		);
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}
