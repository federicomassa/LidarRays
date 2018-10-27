// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CarPlayerController.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLidarDataAvailable, ULidarMessage*, LidarScan);

UCLASS()
class LIDARRAYS_API ACarPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = Lidar)
	FLidarDataAvailable LidarAvailable;

	virtual void BeginPlay() override;
	
};
