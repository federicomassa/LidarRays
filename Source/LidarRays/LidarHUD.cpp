// Fill out your copyright notice in the Description page of Project Settings.

#include "LidarHUD.h"
#include <Engine/World.h>
#include <Math/UnrealMathUtility.h>

void ALidarHUD::BeginPlay() 
{
}

void ALidarHUD::Tick(float DeltaSeconds)
{
	UE_LOG(LogTemp, Warning, TEXT("NICE"));
	Super::Tick(DeltaSeconds);
	DrawRect(FLinearColor::Red, 0, 0, 100, 100);
}

void ALidarHUD::DrawBackground(FLinearColor BGColor, float HUD_X, float HUD_Y, float RectW, float RectH)
{
	DrawRect(BGColor, HUD_X, HUD_Y, RectW, RectH);
	HUDX = HUD_X;
	HUDY = HUD_Y;
	HUDW = RectW;
	HUDH = RectH;
}

void ALidarHUD::DrawLidarScan(FLinearColor PointsColor, const TArray<FVector>& LaserScan)
{
	// We represent a subset of the real points. NB Watch out for periodicity, choose quasi prime numbers
	int NHUDPoints = 101;

	// TODO Here we should use lidar range, but we hardcode it for now, in laser coords
	float MinX = -3100;
	float MaxX = 3100;
	float MinY = -3100;
	float MaxY = 3100;

	// We take a point every PointStep real points
	int PointStep = round(float(LaserScan.Num()) / NHUDPoints);


	int i = 0;

	while(i < LaserScan.Num())
	{
		// Coordinates on background. Should be between 0 and 1
		float ThisPointX = (LaserScan[i].X - MinX) / (MaxX - MinX);
		float ThisPointY = (LaserScan[i].Y - MinY) / (MaxY - MinY);

		//UE_LOG(LogTemp, Warning, TEXT("Point: (%f, %f)"), ThisPointX, ThisPointY);


		if (ThisPointX < 1 && ThisPointX > 0 && ThisPointY < 1 && ThisPointY > 0)
			DrawRect(PointsColor, HUDX + ThisPointY*HUDW, (HUDY + HUDH) - ThisPointX*HUDH, 3, 3);
		// else discard point

		i += PointStep;
	}
}
