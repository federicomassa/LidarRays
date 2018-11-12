// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class UnrealUDP : ModuleRules
{
	public UnrealUDP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"UnrealUDP/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"UnrealUDP/Private",
				// ... add other private include paths required here ...
			}
			);

        // Cereal to serialize sensor data
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "../ThirdParty/cereal/include"));
        bEnableUndefinedIdentifierWarnings = false;

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Networking"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "Core",
                "Networking",
                "Sockets",
                "InputCore"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        //PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost/lib64-msvc-14.1/libboost_system-vc141-mt-x64-1_68.lib"));
        //PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost/lib64-msvc-14.1/libboost_date_time-vc141-mt-x64-1_68.lib"));
        //PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost/lib64-msvc-14.1/libboost_regex-vc141-mt-x64-1_68.lib"));
        //PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost/lib64-msvc-14.1/libboost_serialization-vc141-mt-x64-1_68.lib"));
        //PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost/lib64-msvc-14.1/libboost_program_options-vc141-mt-x64-1_68.lib"));

        //// Boost to serialize
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost"));

        // Needed to package
        bEnableExceptions = true;


    }
}
