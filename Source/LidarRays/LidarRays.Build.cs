// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class LidarRays : ModuleRules
{
	public LidarRays(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Networking", "UnrealUDP", "Slate", "SlateCore", "PhysXVehicles", "HeadMountedDisplay", "LoadingScreen" });

		PrivateDependencyModuleNames.Add("UnrealUDP");

        // Sensor components
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Sensors/Public"));

        // Sensor messages
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Sensors/Messages/Public"));

        // Utility
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Utility/Public"));

        // Vehicle
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Vehicle"));
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Vehicle/Models"));

        //// Simulink communication
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "../ThirdParty/simulink_interface/include"));

        // Cereal to serialize sensor data
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "../ThirdParty/cereal/include"));

        // Boost
        //PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost"));

        //PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost/lib64-msvc-14.1/boost_system-vc141-mt-gd-x64-1_70.lib"));

        //// Disable some warnings
        bEnableUndefinedIdentifierWarnings = false;

        PublicDefinitions.Add("HMD_MODULE_INCLUDED=1");

        bEnableExceptions = true;

        // Needed for dynamic casting
        bUseRTTI = true;


    }
}
