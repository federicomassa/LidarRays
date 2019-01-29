// Fill out your copyright notice in the Description page of Project Settings.

#include "LidarHUD.h"
#include <Engine/World.h>
#include <Math/UnrealMathUtility.h>
#include "LidarMessage.h"

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

void ALidarHUD::DrawLidarScan(FLinearColor PointsColor, const FLidarMessage& LaserScan)
{
	// We represent a subset of the real points. NB Watch out for periodicity, choose quasi prime numbers
	int NHUDPoints = 101;

	if (LaserScan.Points.size() <= NHUDPoints)
		NHUDPoints = LaserScan.Points.size();

	// TODO Here we should use lidar range, but we hardcode it for now, in laser coords
	float MinX = -31; // meters
	float MaxX = 31;
	float MinY = -31;
	float MaxY = 31;



	// We take a point every PointStep real points
	int PointStep = round(float(LaserScan.Points.size()) / NHUDPoints);
	if (PointStep <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No points available for HUD!"));
	}

	int i = 0;

	while (i < LaserScan.Points.size())
	{
		// Coordinates on background. Should be between 0 and 1
		float ThisPointX = (LaserScan.Points[i].x - MinX) / (MaxX - MinX);
		float ThisPointY = (LaserScan.Points[i].y - MinY) / (MaxY - MinY);

		//UE_LOG(LogTemp, Warning, TEXT("Point: (%f, %f)"), ThisPointX, ThisPointY);


		if (ThisPointX < 1 && ThisPointX > 0 && ThisPointY < 1 && ThisPointY > 0)
			//DrawRect(PointsColor, HUDX + ThisPointY * HUDW, (HUDY + HUDH) - ThisPointX * HUDH, 3, 3);
			DrawRect(PointsColor, HUDX + ThisPointX * HUDW, (HUDY + HUDH) - ThisPointY * HUDH, 3, 3);
		// else discard point

		i += PointStep;
	}
}
