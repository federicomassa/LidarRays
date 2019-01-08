// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MessageWrapper.h"
#include "MessageSerializerComponent.generated.h"

class ULidarMessage;
class UIMUMessage;

UCLASS(ClassGroup = (Sensors), meta = (BlueprintSpawnableComponent))
class LIDARRAYS_API UMessageSerializerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMessageSerializerComponent();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = Sensors)
		TArray<uint8> SerializeLidarMessage(ULidarMessage* LidarScan);

	UFUNCTION(BlueprintCallable, Category = Sensors)
		TArray<uint8> SerializeIMUMessage(UIMUMessage* IMUData);

	UFUNCTION(BlueprintCallable, Category = Sensors)
		TArray<uint8> SerializeMessage(UOutgoingSimulinkMessage* Data);


	UFUNCTION(BlueprintCallable, Category = Controller)
		UIncomingSimulinkMessage* DeserializeControlMessage(TArray<uint8> ControlMessage);



};
