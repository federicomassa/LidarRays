// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MessageSerializerComponent.generated.h"

class ULidarMessage;

UCLASS( ClassGroup=(Sensors), meta=(BlueprintSpawnableComponent) )
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

	UFUNCTION(BlueprintCallable, Category = Lidar)
	TArray<uint8> SerializeLidarMessage(ULidarMessage* LidarScan);
	
};
