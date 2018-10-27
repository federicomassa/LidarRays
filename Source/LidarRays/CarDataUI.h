// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LidarMessage.h"
#include "Blueprint/UserWidget.h"
#include "CarDataUI.generated.h"

/**
 * 
 */
UCLASS()
class LIDARRAYS_API UCarDataUI : public UUserWidget
{
	GENERATED_BODY()
	
	void NativePaint(FPaintContext& Context) const override;

	UFUNCTION(BlueprintCallable, Category = SensorData)
	TArray<FVector2D> GetRepresentableLidarPoints(ULidarMessage * LaserScan) const;

};
