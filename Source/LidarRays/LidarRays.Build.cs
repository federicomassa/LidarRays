// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class LidarRays : ModuleRules
{
	public LidarRays(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UnrealUDP", "Slate", "SlateCore", "PhysXVehicles", "HeadMountedDisplay" });

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

        //// Disable some warnings
        bEnableUndefinedIdentifierWarnings = false;

        PublicDefinitions.Add("HMD_MODULE_INCLUDED=1");

        bEnableExceptions = true;

        // Needed for dynamic casting
        bUseRTTI = true;


    }
}
