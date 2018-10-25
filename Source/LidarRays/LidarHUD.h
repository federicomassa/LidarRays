// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LidarHUD.generated.h"


class ULidarMessage;
/**
 * 
 */
UCLASS()
class LIDARRAYS_API ALidarHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float ScreenW = 0.f;

	UPROPERTY(EditAnywhere)
	float ScreenH = 0.f;

	float HUDX = 0.f;
	float HUDY = 0.f;

	float HUDW = 0.f;
	float HUDH = 0.f;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = Lidar)
	void DrawBackground(FLinearColor BGColor, float HUDX, float HUDY, float RectW, float RectH);

	UFUNCTION(BlueprintCallable, Category = Lidar)
	void DrawLidarScan(FLinearColor PointsColor, ULidarMessage* LaserScan);

};
