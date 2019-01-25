// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TazioVehicle.h"
#include "Components/InputComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Engine/Engine.h"
#include "GameFramework/Controller.h"
#include "UObject/ConstructorHelpers.h"
#include "ControlMessage.h"

const FName ATazioVehicle::LookUpBinding("LookUp");
const FName ATazioVehicle::LookRightBinding("LookRight");

#define LOCTEXT_NAMESPACE "VehiclePawn"

ATazioVehicle::ATazioVehicle()
{
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

AUDPReceiver* ATazioVehicle::GetUDPReceiver()
{
	return UDPReceiver;
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
		//InputComponent->BindAxis("MoveForward", this, &ATazioVehicle::MoveForward);
		//InputComponent->BindAxis("MoveRight", this, &ATazioVehicle::MoveRight);
		///*InputComponent->BindAxis(LookUpBinding);
		//InputComponent->BindAxis(LookRightBinding);*/
		//InputComponent->AxisKeyBindings = AxisBindings;
		//UE_LOG(LogTemp, Warning, TEXT("Manual: %i"), InputComponent->AxisBindings.Num());*/
		InputComponent->AxisBindings = AxisBindings;
	}
	else
	{
		InputComponent->AxisBindings.Empty();
	}

}

void ATazioVehicle::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent = PlayerInputComponent;

	//// set up gameplay key bindings
	//check(PlayerInputComponent);

	///*PlayerInputComponent->BindAxis("MoveForward", this, &ATazioVehicle::MoveForward);
	//PlayerInputComponent->BindAxis("MoveRight", this, &ATazioVehicle::MoveRight);
	//PlayerInputComponent->BindAxis(LookUpBinding);
	//PlayerInputComponent->BindAxis(LookRightBinding);*/

	AxisBindings = InputComponent->AxisBindings;
	//UE_LOG(LogTemp, Warning, TEXT("BINDINGS: %i"), InputComponent->AxisBindings.Num());


	PlayerInputComponent->BindAction("ManualDriving", IE_Pressed, this, &ATazioVehicle::ToggleManualDriving);
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


void ATazioVehicle::SendControls(UControlMessage* control)
{
	if (control)
	{
		UE_LOG(LogTemp, Warning, TEXT("Force: %f, Steer: %f"), control->VX, control->Ydot);
		/*if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Force: %f, Steer: %f"), control->VX, control->Ydot);*/
		GetVehicleMovementComponent()->SetThrottleInput(control->VX);
		GetVehicleMovementComponent()->SetSteeringInput(-control->Ydot);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ATazioVehicle::SendControls: Invalid control type message arrived"));
	}
}

void ATazioVehicle::Tick(float Delta)
{
	Super::Tick(Delta);
}

void ATazioVehicle::BeginPlay()
{
	Super::BeginPlay();
}

#undef LOCTEXT_NAMESPACE
