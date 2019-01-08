// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include <fstream>

#include <AllowWindowsPlatformTypes.h>
#include "ilc_cpp.h"
#include <HideWindowsPlatformTypes.h>

#include "CarPlayerController.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLidarDataAvailable, ULidarMessage*, LidarScan);

UCLASS()
class LIDARRAYS_API ACarPlayerController : public APlayerController
{
	GENERATED_BODY()

	void ToggleSaveTrajectory();
	void ToggleILC();
	void ResetPosition();

	bool isSaving = false;
	bool isILCActive = false;
	float ILCStartTime = -1;
	float TrajectoryStartTime = -1;

	ILC::Controller controller;
	std::ofstream output_trajectory;
	float trajectory_frequency = 10; // Hz
	float last_trajectory_time = -100000; // last time a point was inserted in the trajectory

	FVector initial_position;
	FRotator initial_rotation;

public:
	UPROPERTY(BlueprintAssignable, Category = Lidar)
	FLidarDataAvailable LidarAvailable;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

};
