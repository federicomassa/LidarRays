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

        //// Boost to serialize
        //PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost"));

        //// Simulink communication
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "../ThirdParty/simulink_interface/include"));


        // Cereal to serialize sensor data
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "../ThirdParty/cereal/include"));
 

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        //PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost/lib64-msvc-14.1/libboost_system-vc141-mt-x64-1_68.lib"));
        //PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost/lib64-msvc-14.1/libboost_date_time-vc141-mt-x64-1_68.lib"));
        //PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost/lib64-msvc-14.1/libboost_regex-vc141-mt-x64-1_68.lib"));
        //PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost/lib64-msvc-14.1/libboost_serialization-vc141-mt-x64-1_68.lib"));
        //PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost/lib64-msvc-14.1/libboost_program_options-vc141-mt-x64-1_68.lib"));


        //PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost/lib64/libboost_system-vc120-mt-1_58.lib"));
        //PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost/lib64/libboost_date_time-vc120-mt-1_58.lib"));
        //PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost/lib64/libboost_regex-vc120-mt-1_58.lib"));
        //PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "../ThirdParty/boost/lib64/libboost_serialization-vc120-mt-1_58.lib"));

        // Disables some warnings and some unnecessary headers
        //PublicDefinitions.Add("_CRT_SECURE_NO_WARNINGS=1");
        //PublicDefinitions.Add("BOOST_DISABLE_ABI_HEADERS=1");

        //// Disable some warnings
        bEnableUndefinedIdentifierWarnings = false;

        //// Needed configurations in order to run Boost
        //bUseRTTI = true;
        //bEnableExceptions = true;

        PublicDefinitions.Add("HMD_MODULE_INCLUDED=1");

        bEnableExceptions = true;

        // Needed for dynamic casting
        bUseRTTI = true;


    }
}
