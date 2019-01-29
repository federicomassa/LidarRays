// Fill out your copyright notice in the Description page of Project Settings.

#include "CarDataUI.h"
#include <Blueprint/WidgetBlueprintLibrary.h>

void UCarDataUI::NativePaint(FPaintContext& Context) const
{
	Super::NativePaint(Context);
}



TArray<FVector2D> UCarDataUI::GetRepresentableLidarPoints(const FLidarMessage & LaserScan) const
{
	TArray<FVector2D> points;

	// We represent a subset of the real points. NB Watch out for periodicity, choose quasi prime numbers
	int NHUDPoints = 101;

	if (LaserScan.Points.size() <= NHUDPoints)
		NHUDPoints = LaserScan.Points.size();

	// TODO Here we should use lidar range, but we hardcode it for now, in laser coords
	float MinX = -3100;
	float MaxX = 3100;
	float MinY = -3100;
	float MaxY = 3100;



	// We take a point every PointStep real points
	int PointStep = round(float(LaserScan.Points.size()) / NHUDPoints);
	if (PointStep <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No points available for HUD!"));
		return points;
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
			points.Add(FVector2D(ThisPointX, ThisPointY));
		// else discard point

		i += PointStep;
	}

	return points;

}
