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
	//ULidarMessage* Scan = nullptr;

	// Lidar range
	UPROPERTY(EditAnywhere, Category = Lidar)
	float LidarRange = 10000.f;

	// Lidar points yaw spacing
	UPROPERTY(EditAnywhere, Category = Lidar)
	float HorResolution = 0.25; // deg

	UPROPERTY(EditAnywhere, Category = Lidar)
	float VerResolution = 0.8; // deg
	
	UPROPERTY(EditAnywhere, Category = Lidar)
	int HorPoints = 580; // deg

	UPROPERTY(EditAnywhere, Category = Lidar)
	int VerLayers = 4; // deg

	bool isFirst = true;

	float LastLidarScanTime = 0.f; // s

	UPROPERTY(EditAnywhere, Category = Lidar)
	float LidarFrequency = 25; // Hz

	// FIXME const angular separation among lidars. Not true in reality
	UPROPERTY(EditAnywhere, Category = Lidar)
	float HorSeparation = 90; //deg

	// Number of lidar sensors
	UPROPERTY(EditAnywhere, Category = Lidar)
	int NumLidars = 4;

	// First lidar yaw offset with respect to car FIXME correct once HorSeparation has been fixed
	UPROPERTY(EditAnywhere, Category = Lidar)
	float FirstLidarHorOffset = 45;

	// First lidar roll offset with respect to car FIXME correct once lidars have been set up in the generic position
	UPROPERTY(EditAnywhere, Category = Lidar)
	float FirstLidarVerOffset = -1.2;

	// TODO Add custom lidar positioning: for now only fixed offset in Z
	UPROPERTY(EditAnywhere, Category = Lidar)
	float ZOffset = 0.f;

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

	UPROPERTY(BlueprintAssignable, Category = Lidar)
	FLidarAvailableDelegate OnLidarAvailable;

	UPROPERTY(EditAnywhere, Category = Lidar)
	bool DebugLines = false;

	FVector GetLidarLocation() const;
	
};
