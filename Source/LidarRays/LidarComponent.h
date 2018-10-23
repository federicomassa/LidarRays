// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LidarComponent.generated.h"

class AActor;
class UWorld;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LIDARRAYS_API ULidarComponent : public UActorComponent
{
	GENERATED_BODY()

	AActor* Owner = nullptr;
	UWorld* World = nullptr;
	TArray<FVector> Scan;

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

	UFUNCTION(BlueprintCallable, Category = Lidar)
	const TArray<FVector>& GetLatestScan() const
	{
		return Scan;
	}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
