// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LidarMessage.h"
#include "LidarComponent.generated.h"

class AActor;
class UWorld;
class ATazioVehicle;

UCLASS( ClassGroup=(Sensors), meta=(BlueprintSpawnableComponent) )
class LIDARRAYS_API ULidarComponent : public UActorComponent
{
	GENERATED_BODY()

	ATazioVehicle* Owner = nullptr;
	UWorld* World = nullptr;
	//ULidarMessage* Scan = nullptr;

	// Lidar range
	UPROPERTY(EditAnywhere, Category = Lidar)
	float LidarRange = 10000.f; // cm

	// Lidar points yaw spacing
	UPROPERTY(EditAnywhere, Category = Lidar)
	float HorResolution = 0.25; // deg

	UPROPERTY(EditAnywhere, Category = Lidar)
	float VerResolution = 0.8; // deg
	
	UPROPERTY(EditAnywhere, Category = Lidar)
	int HorPoints = 580;

	UPROPERTY(EditAnywhere, Category = Lidar)
	int VerLayers = 2;

	bool isFirst = true;

	// World time at begin play
	float InitTime;

	// Counter of lidar simulations completed. Needed to sync with sensor frequency
	int32 LidarSimulated = 0;

	float LastLidarScanTime = 0.f; // s

	UPROPERTY(EditAnywhere, Category = Lidar)
	float LidarFrequency = 25; // Hz

	// First lidar roll offset with respect to car FIXME correct once lidars have been set up in the generic position
	UPROPERTY(EditAnywhere, Category = Lidar)
	float FirstLidarVerOffset = 0.f;

	UPROPERTY(EditAnywhere, Category = Lidar)
	TArray<FTransform> LidarPoses;

public:	
	// Sets default values for this component's properties
	ULidarComponent();
	~ULidarComponent();

	//UFUNCTION(BlueprintCallable, Category = Lidar)
	//ULidarMessage* GetLatestScan()
	//{
	//	return Scan;
	//}

	UFUNCTION(BlueprintCallable, Category = Lidar)
	float GetLastScanTime()
	{
		return LastLidarScanTime;
	}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = Lidar)
	bool DebugLines = false;

	FVector GetLidarLocation() const;
	
};
