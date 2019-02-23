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
#include "UDPReceiver.h"
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
	UE_LOG(LogTemp, Warning, TEXT("BINDINGS: %i"), InputComponent->AxisBindings.Num());
	UE_LOG(LogTemp, Warning, TEXT("KEY BINDINGS: %i"), InputComponent->AxisKeyBindings.Num());

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

	///*PlayerInputComponent->BindAxis("MoveForward", this, &ATazioVehicle::MoveForward);
	//PlayerInputComponent->BindAxis("MoveRight", this, &ATazioVehicle::MoveRight);
	//PlayerInputComponent->BindAxis(LookUpBinding);
	//PlayerInputComponent->BindAxis(LookRightBinding);*/

	//UE_LOG(LogTemp, Warning, TEXT("BINDINGS: %i"), InputComponent->AxisBindings.Num());

	PlayerInputComponent->BindAxis("Throttle", this, &ATazioVehicle::SetThrottle);
	PlayerInputComponent->BindAxis("Steer", this, &ATazioVehicle::SetSteer);

	PlayerInputComponent->BindAction("ManualDriving", IE_Pressed, this, &ATazioVehicle::ToggleManualDriving);

	AxisBindings = InputComponent->AxisBindings;

}

//void ATazioVehicle::SendControls(UIncomingMessage* msg)
//{
//	TwistMessage<cereal::BinaryInputArchive>* twist = dynamic_cast<TwistMessage<cereal::BinaryInputArchive>*>(msg->message);
//	if (twist)
//	{
//		GetVehicleMovementComponent()->SetThrottleInput(twist->Linear[0]);
//		GetVehicleMovementComponent()->SetSteeringInput(-twist->Angular[2]);
//	}
//	else
//	{
//		UE_LOG(LogTemp, Error, TEXT("ATazioVehicle::SendControls: Invalid control type message arrived"));
//	}
//}


void ATazioVehicle::SendControls(const FControlMessage& control)
{
	//UE_LOG(LogTemp, Warning, TEXT("Force: %f, Steer: %f"), control.VX, control.Ydot);
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Force: %f, Steer: %f"), control->VX, control->Ydot);*/

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
}

void ATazioVehicle::Init()
{
	check(GetGameInstance());
	GameInstance = Cast<UTazioGameInstance>(GetGameInstance());
	check(GameInstance != nullptr);

	// =============== Setup communication ================
	LidarSender = NewObject<AUDPSender>(this);
	LidarSender->Start("Lidar", GameInstance->LidarSendIP, GameInstance->LidarPort, GameInstance->isCommunicationUDP);
	UE_LOG(LogTemp, Warning, TEXT("Lidar: %s:%d"), *GameInstance->LidarSendIP, GameInstance->LidarPort);


	GPSSender = NewObject<AUDPSender>(this);
	GPSSender->Start("GPS", GameInstance->GPSSendIP, GameInstance->GPSPort, GameInstance->isCommunicationUDP);
	UE_LOG(LogTemp, Warning, TEXT("GPS: %s:%d"), *GameInstance->GPSSendIP, GameInstance->GPSPort);

	GPSTruthSender = NewObject<AUDPSender>(this);
	GPSTruthSender->Start("GPSTruth", GameInstance->GPSTruthSendIP, GameInstance->GPSTruthPort, GameInstance->isCommunicationUDP);
	UE_LOG(LogTemp, Warning, TEXT("GPS Truth: %s:%d"), *GameInstance->GPSTruthSendIP, GameInstance->GPSTruthPort);

	IMUSender = NewObject<AUDPSender>(this);
	IMUSender->Start("IMU", GameInstance->IMUSendIP, GameInstance->IMUPort, GameInstance->isCommunicationUDP);
	UE_LOG(LogTemp, Warning, TEXT("IMU: %s:%d"), *GameInstance->IMUSendIP, GameInstance->IMUPort);

	ControlReceiver = NewObject<AUDPReceiver>(this);
	ControlReceiver->Start("Control", GameInstance->ControlReceiveIP, GameInstance->ControlPort);
	UE_LOG(LogTemp, Warning, TEXT("Control: %s:%d"), *GameInstance->ControlReceiveIP, GameInstance->ControlPort);

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
