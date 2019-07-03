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
#include "MessageSerializerComponent.h"
#include "LidarComponent.h"

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
constexpr int MAX_VEHICLES = 4;

ATazioVehicle::ATazioVehicle()
{
	GPSComponent = CreateDefaultSubobject<UGPSComponent>(TEXT("GPS"));
	GPSComponent->SetAutoActivate(false);
	GPSComponent->Deactivate();
	GPSComponent->PrimaryComponentTick.SetTickFunctionEnable(false);

	LidarComponent = CreateDefaultSubobject<ULidarComponent>(TEXT("Lidar"));
	LidarComponent->SetAutoActivate(false);
	LidarComponent->Deactivate();
	LidarComponent->PrimaryComponentTick.SetTickFunctionEnable(false);

	MessageSerializerComponent = CreateDefaultSubobject<UMessageSerializerComponent>(TEXT("MessageSerializer"));
	MessageSerializerComponent->Deactivate();

	debug_file.open("D:/debug.txt");
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

	// TEMP
	/* UGPSComponent* GPSComponent = FindComponentByClass<UGPSComponent>();
	check(GPSComponent);

	PlayerInputComponent->BindAction("ToggleGPS", IE_Pressed, GPSComponent, &UGPSComponent::ToggleGPS);

	PlayerInputComponent->BindAction("RecordTrajectory", IE_Pressed, this, &ATazioVehicle::ToggleRecordTrajectory);


	AxisBindings = InputComponent->AxisBindings;*/
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

		//debug_file << pose.timestamp << '\t' << currentTime << '\n';
	}
}

void ATazioVehicle::ToggleRecordTrajectory()
{
	isRecordingTrajectory = !isRecordingTrajectory;
}

void ATazioVehicle::Tick(float Delta)
{
	Super::Tick(Delta);

	// If player settings are not available yet, return
	if (!_PlayerSettingsReady && !WaitForPlayerSettings())
		return;

	if (!_IDReceived)
		OnIDChanged.Broadcast(_ID);

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


	//if (currentPose)
	//{
	//	SetActorLocation(FVector(currentPose->x * 100, -currentPose->y * 100, 150.0));
	//	SetActorRotation(FRotator(0.f, -currentPose->theta * 180 / 3.14159, 0.f));
	//}
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

void ATazioVehicle::SetID(const int& id)
{
	_ID = id;

	// Broadcast event to change text box above car
	OnIDChanged.Broadcast(_ID);

	UE_LOG(LogTemp, Warning, TEXT("Setting ID %d for player %s"), id, *GetActorLabel());
}

void ATazioVehicle::SetPlayerIndex(const int& PlayerIndex)
{
	_PlayerIndex = PlayerIndex;
}

bool ATazioVehicle::WaitForPlayerSettings()
{
	// Check if this player has its player settings available
	if (GameInstance->PlayersSettings.Contains(_PlayerIndex)) {

		auto& PlayerSettings = GameInstance->PlayersSettings[_PlayerIndex];

		PoseReceiver = NewObject<AUDPReceiver>(this);
		PoseReceiver->Start("PoseReceiver",
			PlayerSettings.VehicleIP,
			PlayerSettings.VehiclePort);

		LidarComponent->SetActive(PlayerSettings.bIsLidarActive);
		LidarComponent->PrimaryComponentTick.SetTickFunctionEnable(PlayerSettings.bIsLidarActive);

		GPSComponent->SetActive(PlayerSettings.bIsGPSActive);
		GPSComponent->PrimaryComponentTick.SetTickFunctionEnable(PlayerSettings.bIsGPSActive);

		if (PlayerSettings.bIsLidarActive) {

			LidarSender = NewObject<AUDPSender>(this);
			LidarSender->Start("Lidar", PlayerSettings.LidarSendIP, PlayerSettings.LidarSendPort, GameInstance->isCommunicationUDP);
		}

		if (PlayerSettings.bIsGPSActive) {
			GPSSender = NewObject<AUDPSender>(this);
			GPSSender->Start("GPS", PlayerSettings.GPSSendIP, PlayerSettings.GPSSendPort, GameInstance->isCommunicationUDP);
		}

		SensManager = NewObject<USensorManager>();
		SensManager->Init(this);

		_PlayerSettingsReady = true;

		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Players settings for player index %d are not available!"), _PlayerIndex);

		return false;
	}
}

void ATazioVehicle::IDReceived()
{
	_IDReceived = true;
}

void ATazioVehicle::SetGPSActive(bool bNewActive)
{
	GPSComponent->SetActive(bNewActive);

	if (bNewActive)
	{
		if (GPSSender == nullptr)
			GPSSender = NewObject<AUDPSender>(this);

		auto& PlayerSettings = GameInstance->PlayersSettings[_PlayerIndex];

		GPSSender->Start("GPS", PlayerSettings.GPSSendIP, PlayerSettings.GPSSendPort, GameInstance->isCommunicationUDP);
	}
}

void ATazioVehicle::SetLidarActive(bool bNewActive)
{
	LidarComponent->SetActive(bNewActive);

	if (bNewActive)
	{
		if (LidarSender == nullptr)
			LidarSender = NewObject<AUDPSender>(this);

		auto& PlayerSettings = GameInstance->PlayersSettings[_PlayerIndex];

		LidarSender->Start("Lidar", PlayerSettings.LidarSendIP, PlayerSettings.LidarSendPort, GameInstance->isCommunicationUDP);
	}
}


#undef LOCTEXT_NAMESPACE
