// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TazioVehicle.h"
#include "Components/InputComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Engine/Engine.h"
#include "GameFramework/Controller.h"
#include "UObject/ConstructorHelpers.h"
#include "MessageWrapper.h"
#include "TestMessage.h"

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

	//// Serialize
	//UOutgoingMessage* toSerialize = NewObject<UOutgoingMessage>();
	//std::ostringstream oss;
	//cereal::BinaryOutputArchive oa(oss);
	//toSerialize->message = new TestMessage<cereal::BinaryOutputArchive>;

	//oa(*toSerialize->message);

	//UE_LOG(LogTemp, Warning, TEXT("Sending message: %s"), *FString(toSerialize->message->ToString().c_str()));

	//// Unserialize
	//UIncomingMessage* toUnserialize = NewObject<UIncomingMessage>();
	//std::istringstream iss;
	//iss.str(oss.str());

	//cereal::BinaryInputArchive ia(iss);
	//toUnserialize->message = new TestMessage<cereal::BinaryInputArchive>;

	//ia(*toUnserialize->message);
	//UE_LOG(LogTemp, Warning, TEXT("Arriving message: %s"), *FString(toUnserialize->message->ToString().c_str()));
}

#undef LOCTEXT_NAMESPACE
