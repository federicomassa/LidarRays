// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TazioVehicle.h"
#include "Components/InputComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Engine/Engine.h"
#include "GameFramework/Controller.h"
#include "KinematicMovementComponent.h"
#include "TazioGameInstance.h"
#include <GCObjectScopeGuard.h>
#include "SensorManager.h"
#include "UDPSender.h"
#include "ControlMessage.h"
#include <iomanip>
#include "UDPReceiver.h"
#include "VehicleModel.h"
#include "GPSComponent.h"
#include <array>
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "ControlMessage.h"
#include <Components/SceneCaptureComponent2D.h>
#include <Engine/TextureRenderTarget2D.h>
#include <chrono>

const FName ATazioVehicle::LookUpBinding("LookUp");
const FName ATazioVehicle::LookRightBinding("LookRight");

#define LOCTEXT_NAMESPACE "VehiclePawn"

ATazioVehicle::ATazioVehicle()
{
	debug_file.open("D:/debug.txt");
	
	const int MAX_VEHICLES = 4;
	for (int i = 0; i < MAX_VEHICLES; i++)
	{
		ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RenderTargetObj(*(FString("/Game/Maps/Cameras/ThirdPersonVehicle_") + FString::FromInt(i)));
		check(RenderTargetObj.Succeeded());

		TextureTargets.Add(RenderTargetObj.Object);
	}
}

ATazioVehicle::~ATazioVehicle()
{
	if (trajectory_dump.is_open())
		trajectory_dump.close();

	if (debug_file.is_open())
		debug_file.close();
}

USensorManager* ATazioVehicle::GetSensorManager()
{
	return SensManager;
}

AUDPSender* ATazioVehicle::GetLidarSender()
{
	return LidarSender;
}

AUDPSender* ATazioVehicle::GetGPSSender()
{
	return GPSSender;
}

AUDPSender* ATazioVehicle::GetGPSTruthSender()
{
	return GPSTruthSender;
}

AUDPSender* ATazioVehicle::GetIMUSender()
{
	return IMUSender;
}

AUDPReceiver* ATazioVehicle::GetControlReceiver()
{
	return ControlReceiver;
}

AUDPReceiver* ATazioVehicle::GetPoseReceiver()
{
	return PoseReceiver;
}

void ATazioVehicle::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent = PlayerInputComponent;

	//// set up gameplay key bindings
	//check(PlayerInputComponent);

	UGPSComponent* GPSComponent = FindComponentByClass<UGPSComponent>();
	check(GPSComponent);

	PlayerInputComponent->BindAction("ToggleGPS", IE_Pressed, GPSComponent, &UGPSComponent::ToggleGPS);

	PlayerInputComponent->BindAction("RecordTrajectory", IE_Pressed, this, &ATazioVehicle::ToggleRecordTrajectory);


	AxisBindings = InputComponent->AxisBindings;
}

void ATazioVehicle::SendControls(const FControlMessage& control)
{

}

void ATazioVehicle::SendPose(const FPoseMessage& pose)
{
	// Check if vehicle simulator has been reset
	if (currentPose && lastPose && currentPose->timestamp < lastPose->timestamp)
	{
		initTime = std::chrono::steady_clock::now();
		currentPose.Release();
		lastPose.Release();
	}

	
	// Init case
	if (!currentPose)
	{
		currentPose = TUniquePtr<FPoseMessage>(new FPoseMessage);
		*currentPose = pose;
		initTime = std::chrono::steady_clock::now();

		currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::steady_clock::now() - initTime).count()*1E-9;
	}
	else
	{
		// Init
		if (!lastPose)
			lastPose = TUniquePtr<FPoseMessage>(new FPoseMessage);

		// Store old value in lastPose
		*lastPose = *currentPose;
		*currentPose = pose;

		lastTime = currentTime;

		currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::steady_clock::now() - initTime).count()*1E-9;

		debug_file << pose.timestamp << '\t' << currentTime << '\n';
	}
}

void ATazioVehicle::ToggleRecordTrajectory()
{
	isRecordingTrajectory = !isRecordingTrajectory;
}

void ATazioVehicle::Tick(float Delta)
{
	Super::Tick(Delta);

	double now = std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::steady_clock::now() - initTime).count()*1E-9;

	double x_interp, y_interp, theta_interp;

	if (lastPose)
	{
		// Estimate pose with linear interpolation
		x_interp = lastPose->x + (currentPose->x - lastPose->x) / (currentPose->timestamp - lastPose->timestamp)*(now - lastTime);
		y_interp = lastPose->y + (currentPose->y - lastPose->y) / (currentPose->timestamp - lastPose->timestamp)*(now - lastTime);

		// TODO unwrap
		theta_interp = currentPose->theta;
	}
	else if (currentPose)
	{
		x_interp = currentPose->x;
		y_interp = currentPose->y;
		theta_interp = currentPose->theta;
	}
	else
	{
		x_interp = 0.0;
		y_interp = 0.0;
		theta_interp = 0.0;
	}

	SetActorLocation(FVector(x_interp*100, -y_interp*100, 150.0));
	SetActorRotation(FRotator(0.f, -theta_interp*180/3.14159, 0.f));

	// Dump trajectory to csv file
	if (isRecordingTrajectory && trajectory_dump.good())
	{
		if (isFirst)
		{
			InitRecordingTime = GetWorld()->GetTimeSeconds();
			isFirst = false;
		}
		else
		{
			trajectory_dump << '\n';
		}

		FVector Location = GetActorLocation();
		float Yaw = GetActorRotation().Yaw;

		trajectory_dump << double(GetWorld()->GetTimeSeconds()) - double(InitRecordingTime) << ',';
		trajectory_dump << double(Location.X)/100.0 << ',';
		trajectory_dump << double(Location.Y)/100.0 << ',';
		trajectory_dump << double(Location.Z)/100.0 << ',';
		trajectory_dump << double(Yaw)*PI/180.0 << ',';
		trajectory_dump << double(GetVelocity().X)/100.0;
	}
} 

void ATazioVehicle::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("Initializing actor with label %s"), *GetActorLabel());


	check(GetGameInstance());
	GameInstance = Cast<UTazioGameInstance>(GetGameInstance());
	check(GameInstance != nullptr);

	// =============== Setup communication ================
	LidarSender = NewObject<AUDPSender>(this);
	LidarSender->Start("Lidar", GameInstance->LidarSendIP, GameInstance->LidarPort, GameInstance->isCommunicationUDP);

	GPSSender = NewObject<AUDPSender>(this);
	GPSSender->Start("GPS", GameInstance->GPSSendIP, GameInstance->GPSPort, GameInstance->isCommunicationUDP);

	GPSTruthSender = NewObject<AUDPSender>(this);
	GPSTruthSender->Start("GPSTruth", GameInstance->GPSTruthSendIP, GameInstance->GPSTruthPort, GameInstance->isCommunicationUDP);

	IMUSender = NewObject<AUDPSender>(this);
	IMUSender->Start("IMU", GameInstance->IMUSendIP, GameInstance->IMUPort, GameInstance->isCommunicationUDP);

	ControlReceiver = NewObject<AUDPReceiver>(this);
	ControlReceiver->Start("Control", GameInstance->ControlReceiveIP, GameInstance->ControlPort);

	PoseReceiver = NewObject<AUDPReceiver>(this);

	//PlayerIndex = GetPlayerIndex();

	//// Check that pose is set to be received
	//check(GameInstance->PoseReceiveEndpoints.Num() > PlayerIndex);
	//PoseReceiver->Start("PoseReceiver", 
	//	GameInstance->PoseReceiveEndpoints[PlayerIndex].Address,
	//	GameInstance->PoseReceiveEndpoints[PlayerIndex].Port);

	SensManager = NewObject<USensorManager>();
	SensManager->Init(this);
}

void ATazioVehicle::BeginPlay()
{
	Super::BeginPlay();

	Init();

	UTazioGameInstance* GameInstance = Cast<UTazioGameInstance>(GetGameInstance());
	check(GameInstance);

	FString DumpPath = GameInstance->DumpTrajectory;
	trajectory_dump.open(TCHAR_TO_ANSI(*DumpPath));
	trajectory_dump << std::fixed << std::setprecision(10);

	Mesh = FindComponentByClass<USkeletalMeshComponent>();
	PhysicsMovementComponent = FindComponentByClass<UWheeledVehicleMovementComponent4W>();
	/*ThirdPersonSceneComponent = NewObject<USceneCaptureComponent2D>(this);
	AddOwnedComponent(ThirdPersonSceneComponent);

	ThirdPersonSceneComponent->TextureTarget = TextureTargets[PlayerIndex];
	FVector CameraLocation = Mesh->GetSocketLocation("ThirdPersonCamera");
	FRotator CameraRotation = Mesh->GetSocketRotation("ThirdPersonCamera");

	ThirdPersonSceneComponent->SetAbsolute(true, true);
	ThirdPersonSceneComponent->SetWorldLocationAndRotation(CameraLocation, CameraRotation);
	ThirdPersonSceneComponent->Activate();*/

}

void ATazioVehicle::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

int ATazioVehicle::GetPlayerIndex()
{
	int index = -1;

	// Actor should be named Player_%d. TODO Only for Player_0 lidars/cameras are simulated
	FRegexPattern PlayerNamePattern("Player_([0-9]+)");
	FRegexMatcher PlayerNameMatcher(PlayerNamePattern, GetActorLabel());

	if (PlayerNameMatcher.FindNext())
	{
		FString Match = PlayerNameMatcher.GetCaptureGroup(1);
		UE_LOG(LogTemp, Warning, TEXT("Matching: %s"), *Match);
		check(Match.IsNumeric());
		index = FCString::Atoi(*Match);
	}

	return index;
}

TArray<UTextureRenderTarget2D*> ATazioVehicle::GetTextureTargets() const
{
	return TextureTargets;
}



#undef LOCTEXT_NAMESPACE
