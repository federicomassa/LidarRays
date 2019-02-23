// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include "SensorManager.generated.h"

struct FControlMessage;
struct FLidarMessage;
struct FIMUMessage;
struct FOdometryMessage;
class UMessageSerializerComponent;
class AUDPSender;
class AUDPReceiver;
class ATazioVehicle;

UCLASS()
class LIDARRAYS_API USensorManager : public UObject
{
	GENERATED_BODY()

	UPROPERTY()
	ATazioVehicle* Owner;

public:
	USensorManager();

	void Init(ATazioVehicle*);

	void SendLidarMessage(FLidarMessage);
	void SendIMUMessage(FIMUMessage);
	void SendGPSMessage(FOdometryMessage);
	void SendGPSTruthMessage(FOdometryMessage);

	UFUNCTION()
	void ReceiveControlMessage(const TArray<uint8>& data);

	// NB not making them UPROPERTY because they are set from TazioVehicle and if that is destroyed, then these do not make sense anymore.

	AUDPSender* LidarSender = nullptr;

	AUDPSender* IMUSender = nullptr;

	AUDPSender* GPSSender = nullptr;

	AUDPSender* GPSTruthSender = nullptr;

	AUDPReceiver* ControlReceiver = nullptr;

	UMessageSerializerComponent* MessageSerializerComponent = nullptr;
};
