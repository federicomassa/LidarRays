// Fill out your copyright notice in the Description page of Project Settings.

#include "SensorManager.h"
#include "LidarMessage.h"
#include "IMUMessage.h"
#include "OdometryMessage.h"
#include "TazioVehicle.h"
#include "UDPSender.h"
#include "UDPReceiver.h"
#include "MessageSerializerComponent.h"

USensorManager::USensorManager()
{

}

void USensorManager::Init(ATazioVehicle* Owner)
{
	check(Owner);
	this->Owner = Owner;
	MessageSerializerComponent = Owner->FindComponentByClass<UMessageSerializerComponent>();

	check(MessageSerializerComponent);

	LidarSender = Owner->GetLidarSender();
	check(LidarSender);

	IMUSender = Owner->GetIMUSender();
	check(IMUSender);

	GPSSender = Owner->GetGPSSender();
	check(GPSSender);

	GPSTruthSender = Owner->GetGPSTruthSender();
	check(GPSTruthSender);

	ControlReceiver = Owner->GetControlReceiver();
	check(ControlReceiver);

	ControlReceiver->OnDataReceived.AddDynamic(this, &USensorManager::ReceiveControlMessage);
}

void USensorManager::SendLidarMessage(FLidarMessage msg)
{
	TArray<uint8> data = MessageSerializerComponent->SerializeLidarMessage(msg);
	LidarSender->SendData(data);
}

void USensorManager::SendIMUMessage(FIMUMessage msg)
{
	TArray<uint8> data = MessageSerializerComponent->SerializeIMUMessage(msg);
	IMUSender->SendData(data);
}

void USensorManager::SendGPSMessage(FOdometryMessage msg)
{
	TArray<uint8> data = MessageSerializerComponent->SerializeOdometryMessage(msg);
	GPSSender->SendData(data);
}

void USensorManager::SendGPSTruthMessage(FOdometryMessage msg)
{
	TArray<uint8> data = MessageSerializerComponent->SerializeOdometryMessage(msg);
	GPSTruthSender->SendData(data);
}

void USensorManager::ReceiveControlMessage(const TArray<uint8>& data)
{
	FControlMessage msg = MessageSerializerComponent->DeserializeControlMessage(data);
	Owner->SendControls(msg);
}


