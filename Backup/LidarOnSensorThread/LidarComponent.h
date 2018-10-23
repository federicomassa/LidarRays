// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LidarComponent.generated.h"

class AMyPawn;
class UWorld;
//class FSensorWorker;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LIDARRAYS_API ULidarComponent : public UActorComponent
{
	GENERATED_BODY()

	AMyPawn* Owner = nullptr;
	UWorld* World = nullptr;
	//FSensorWorker* SensorWorker = nullptr;
	TArray<FVector> Scan;

	UPROPERTY(EditAnywhere, Category = Lidar)
	float LidarRange = 1000.f;
	bool isFirst = true;

	float LastLidarScanTime = 0.f; // s
	float LidarFrequency = 10; // Hz

public:	
	// Sets default values for this component's properties
	ULidarComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
