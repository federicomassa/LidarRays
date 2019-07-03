// Fill out your copyright notice in the Description page of Project Settings.

#include "SensorManager.h"
#include "LidarMessage.h"
#include "IMUMessage.h"
#include "OdometryMessage.h"
#include "TazioVehicle.h"
#include "UDPSender.h"
#include "UDPReceiver.h"
#include "PoseMessage.h"
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

	IMUSender = Owner->GetIMUSender();

	GPSSender = Owner->GetGPSSender();

	GPSTruthSender = Owner->GetGPSTruthSender();

	ControlReceiver = Owner->GetControlReceiver();

	PoseReceiver = Owner->GetPoseReceiver();

	if (PoseReceiver)
		PoseReceiver->OnDataReceived.AddDynamic(this, &USensorManager::ReceivePoseMessage);
	else
		UE_LOG(LogTemp, Error, TEXT("Pose receiver not found in owner!"));
}

void USensorManager::SendLidarMessage(FLidarMessage msg)
{
	if (!IsLidarAvailable())
	{
		UE_LOG(LogTemp, Error, TEXT("Trying to send lidar message but lidar sender is not available!"));
		return;
	}

	TArray<uint8> data = MessageSerializerComponent->SerializeLidarMessage(msg);
	LidarSender->SendData(data);
}

void USensorManager::SendIMUMessage(FIMUMessage msg)
{
	TArray<uint8> data = MessageSerializerComponent->SerializeIMUMessage(msg, true);
	IMUSender->SendData(data);
}

void USensorManager::SendGPSMessage(FOdometryMessage msg)
{
	if (!IsGPSAvailable())
	{
		UE_LOG(LogTemp, Error, TEXT("Trying to send gps message but gps sender is not available!"));
		return;
	}

	TArray<uint8> data = MessageSerializerComponent->SerializeOdometryMessage(msg, true);
	GPSSender->SendData(data);
}

void USensorManager::SendGPSTruthMessage(FOdometryMessage msg)
{
	TArray<uint8> data = MessageSerializerComponent->SerializeOdometryMessage(msg, true);
	GPSTruthSender->SendData(data);
}

void USensorManager::ReceiveControlMessage(const TArray<uint8>& data)
{
	FControlMessage msg = MessageSerializerComponent->DeserializeControlMessage(data, true);
	Owner->SendControls(msg);
}

void USensorManager::ReceivePoseMessage(const TArray<uint8>& data)
{
	FPoseMessage msg = MessageSerializerComponent->DeserializePoseMessage(data, true);
	Owner->SendPose(msg);
}

