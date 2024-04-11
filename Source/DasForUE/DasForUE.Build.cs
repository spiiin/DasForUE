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
		PublicAdditionalLibraries.Add(Path.Combine(PluginDir, "ThirdParty/daScript/cmake_temp/Release/libDaScript.lib"));
		
		PublicDefinitions.Add("DAS_EXTERN_LOGERROR");
		PublicIncludePaths.AddRange(
			new string[] {
			}
		);

		
		PrivateIncludePaths.AddRange(
			new string[] {
				"../ThirdParty/daScript/include"
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
				"Kismet",
				"Slate",
				"SlateCore",
				
				"Projects", //IPluginManager
			}
		);
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}
