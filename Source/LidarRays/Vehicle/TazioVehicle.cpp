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
#include "UDPReceiver.h"
#include "VehicleModel.h"
#include "GPSComponent.h"
#include <array>
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "ControlMessage.h"

const FName ATazioVehicle::LookUpBinding("LookUp");
const FName ATazioVehicle::LookRightBinding("LookRight");

#define LOCTEXT_NAMESPACE "VehiclePawn"

ATazioVehicle::ATazioVehicle()
{
}

ATazioVehicle::~ATazioVehicle()
{
	if (DynamicModel)
		delete DynamicModel;

	if (trajectory_dump.is_open())
		trajectory_dump.close();
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

void ATazioVehicle::ToggleManualDriving()
{
	if (InputComponent->AxisBindings.Num() > 0)
		AxisBindings = InputComponent->AxisBindings;

	ManualDriving = !ManualDriving;

	if (ManualDriving)
	{
		///*InputComponent->BindAxis(LookUpBinding);
		//InputComponent->BindAxis(LookRightBinding);*/
		//InputComponent->AxisKeyBindings = AxisBindings;
		//UE_LOG(LogTemp, Warning, TEXT("Manual: %i"), InputComponent->AxisBindings.Num());*/
		InputComponent->AxisBindings = AxisBindings;
		UE_LOG(LogTemp, Warning, TEXT("Manual mode activated"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AI control mode activated"));
		InputComponent->AxisBindings.Empty();
	}

}

void ATazioVehicle::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent = PlayerInputComponent;

	UE_LOG(LogTemp, Warning, TEXT("Setting Player input: %i"), bPhysXSimulation);

	//// set up gameplay key bindings
	//check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Throttle", this, &ATazioVehicle::SetThrottle);
	PlayerInputComponent->BindAxis("Steer", this, &ATazioVehicle::SetSteer);

	PlayerInputComponent->BindAction("ManualDriving", IE_Pressed, this, &ATazioVehicle::ToggleManualDriving);

	UGPSComponent* GPSComponent = FindComponentByClass<UGPSComponent>();
	check(GPSComponent);

	PlayerInputComponent->BindAction("ToggleGPS", IE_Pressed, GPSComponent, &UGPSComponent::ToggleGPS);

	PlayerInputComponent->BindAction("RecordTrajectory", IE_Pressed, this, &ATazioVehicle::ToggleRecordTrajectory);


	AxisBindings = InputComponent->AxisBindings;
}

void ATazioVehicle::SendControls(const FControlMessage& control)
{
	//UE_LOG(LogTemp, Warning, TEXT("Force: %f, Steer: %f"), control.VX, control.Ydot);

	if (bPhysXSimulation)
	{
		GetVehicleMovementComponent()->SetThrottleInput(control.VX);
		GetVehicleMovementComponent()->SetSteeringInput(-control.Ydot);
	}
	else
	{	
		controls.insert(control);
	}
}

void ATazioVehicle::ToggleRecordTrajectory()
{
	isRecordingTrajectory = !isRecordingTrajectory;
}

void ATazioVehicle::Tick(float Delta)
{
	Super::Tick(Delta);
	//UE_LOG(LogTemp, Warning, TEXT("TICK!"));

	lastDeltaTime = Delta;
	if (IsManualDriveMode())
	{
		FControlMessage control;
		control.VX = lastThrottle;
		control.Ydot = lastSteer;

		controls.insert(control);
	}

	if (IsManualDriveMode() && bPhysXSimulation)
	{ 
		FControlMessage control;
		control.VX = lastThrottle;
		control.Ydot = lastSteer;

		SendControls(control);
	}
	else if (!bPhysXSimulation && !(controls.isEmpty()))
	{
		FControlMessage control = controls.pop();

		std::map<std::string, double> originalControls;
		originalControls["Throttle"] = control.VX;
		originalControls["Steering"] = control.Ydot;


		DynamicModel->run(originalControls, Delta);

		std::map<std::string, double> currentState = DynamicModel->getWorldState();

		FRotator currentRotation = GetActorRotation();

		//Mesh->SetWorldLocation(FVector(currentState.at("x"), currentState.at("y"), Mesh->GetWorldLocation().Z));
		//Mesh->SetWorldRotation(FQuat(FRotator(currentRotation.Pitch, currentState.at("yaw"), currentRotation.Roll)));

		SetActorLocation(FVector(currentState.at("x"), currentState.at("y"), GetActorLocation().Z));
		SetActorRotation(FQuat(FRotator(currentRotation.Pitch, currentState.at("yaw"), currentRotation.Roll)));

		std::array<double, 3> velocity = DynamicModel->getVelocity();
		//UE_LOG(LogTemp, Warning, TEXT("SETTING VELOCITY: %f, %f, %f"), velocity[0], velocity[1], velocity[2]);
		ModelMovementComponent->Velocity = FVector(velocity[0], velocity[1], velocity[2]);
	}

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

		trajectory_dump << GetWorld()->GetTimeSeconds() - InitRecordingTime << ',';
		trajectory_dump << Location.X/100.f << ',';
		trajectory_dump << Location.Y/100.f << ',';
		trajectory_dump << Location.Z/100.f << ',';
		trajectory_dump << Yaw*PI/180.f << ',';
		trajectory_dump << GetVelocity().X/100.f;
	}
} 

void ATazioVehicle::Init()
{
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

	// ===================== Vehicle Model ============================
	VehicleModelType = GameInstance->VehicleModel;
	DynamicModel = VehicleModel::generateVehicleModel(VehicleModelType);

	if (VehicleModelType != EVehicleModelEnum::VM_PhysX)
		bPhysXSimulation = false;

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

	Mesh = FindComponentByClass<USkeletalMeshComponent>();
	PhysicsMovementComponent = FindComponentByClass<UWheeledVehicleMovementComponent4W>();

	if (DynamicModel)
	{
		// Remove physics movement component, add model movement component
		Mesh->SetSimulatePhysics(false);


		PhysicsMovementComponent->DestroyComponent();

		ModelMovementComponent = NewObject<UKinematicMovementComponent>(this);
		AddOwnedComponent(ModelMovementComponent);

	}

	if (DynamicModel)
	{
		DynamicModel->initModel();

		// Create pawn movement component
		std::map<std::string, double> initState;
		FVector initLocation = GetActorLocation();
		FRotator initRotation = GetActorRotation();

		initState["x"] = initLocation.X;
		initState["y"] = initLocation.Y;
		initState["v"] = 0.f;
		initState["yaw"] = initRotation.Yaw;

		DynamicModel->setState(initState);

		std::map<std::string, double> newState = DynamicModel->getWorldState();
		FVector newWorldState;
		newWorldState.X = newState.at("x");
		newWorldState.Y = newState.at("y");
		newWorldState.Z = initLocation.Z + 10.f;

		SetActorLocation(newWorldState, false, nullptr, ETeleportType::TeleportPhysics);
		SetActorRotation(FQuat(FRotator(initRotation.Pitch, newState.at("yaw"), initRotation.Roll)), ETeleportType::TeleportPhysics);
	}
}

void ATazioVehicle::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (DynamicModel)
		DynamicModel->closeModel();

	Super::EndPlay(EndPlayReason);
}

void ATazioVehicle::SetThrottle(float value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Throttle"));
	lastThrottle = value;
}

void ATazioVehicle::SetSteer(float value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Steer"));
	lastSteer = -value;
}



#undef LOCTEXT_NAMESPACE
