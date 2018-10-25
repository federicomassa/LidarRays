// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LidarMessage.h"
#include "LidarComponent.generated.h"

class AActor;
class UWorld;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLidarAvailableDelegate, ULidarMessage*, LidarScan);

UCLASS( ClassGroup=(Sensors), meta=(BlueprintSpawnableComponent) )
class LIDARRAYS_API ULidarComponent : public UActorComponent
{
	GENERATED_BODY()

	AActor* Owner = nullptr;
	UWorld* World = nullptr;
	ULidarMessage* Scan = nullptr;

	// Lidar range
	UPROPERTY(EditAnywhere, Category = Lidar)
	float LidarRange = 10000.f;

	// Lidar points yaw spacing
	UPROPERTY(EditAnywhere, Category = Lidar)
	float DeltaYaw = 0.1; // deg

	bool isFirst = true;

	float LastLidarScanTime = 0.f; // s
	float LidarFrequency = 25; // Hz

public:	
	// Sets default values for this component's properties
	ULidarComponent();
	~ULidarComponent();

	UFUNCTION(BlueprintCallable, Category = Lidar)
	ULidarMessage* GetLatestScan()
	{
		return Scan;
	}

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

	UPROPERTY(BlueprintAssignable, Category = Lidar)
	FLidarAvailableDelegate OnLidarAvailable;
	
};
