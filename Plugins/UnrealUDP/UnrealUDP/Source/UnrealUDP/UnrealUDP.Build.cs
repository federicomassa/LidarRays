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

       

    }
}
