// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TazioVehicle.h"
#include "Components/InputComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Engine/Engine.h"
#include "GameFramework/Controller.h"
#include "UObject/ConstructorHelpers.h"

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

void ATazioVehicle::ControlForward(float Val)
{
	UE_LOG(LogTemp, Warning, TEXT("Received forward!"));
	GetVehicleMovementComponent()->SetThrottleInput(Val);
}

void ATazioVehicle::ControlRight(float Val)
{
	GetVehicleMovementComponent()->SetSteeringInput(Val);
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
