// Fill out your copyright notice in the Description page of Project Settings.

#include "LidarComponent.h"
#include "MyPawn.h"
#include <Engine/World.h>
#include <GameFramework/Actor.h>


// Sets default values for this component's properties
ULidarComponent::ULidarComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}


// Called when the game starts
void ULidarComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	World = GetWorld();

	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("Lidar component has no owner"));
	}

	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Lidar component has no world"));
	}
}


// Called every frame
void ULidarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//UE_LOG(LogTemp, Warning, TEXT("FPS: %f"), 1.f / DeltaTime);


	int PointsPerLayer = 600;
	int Layers = 4;
	int NLidars = 4;

	int TotalPoints = PointsPerLayer * Layers* NLidars;

	// Request lidar scan depending on sensor frequency
	if (isFirst || World->GetTimeSeconds() - LastLidarScanTime > 1.f / LidarFrequency)
	{

		Scan.Empty();

		/*if (!isFirst)
			UE_LOG(LogTemp, Warning, TEXT("Lidar frequency: %f"), 1.f / (World->GetTimeSeconds() - LastLidarScanTime));*/

		// Update last scan time
		LastLidarScanTime = World->GetTimeSeconds();

		FVector CurrentLocation = Owner->GetActorLocation();
		FRotator CurrentRotation = Owner->GetActorRotation();
		FHitResult Hit;

		for (int i = 0; i < TotalPoints; i++)
		{
			FRotator ThisRotation = CurrentRotation + FRotator(0, i*DeltaYaw, 0);
			FVector EndLocation = CurrentLocation + LidarRange * (ThisRotation.Vector());

			World->LineTraceSingleByChannel(
				Hit,
				CurrentLocation,
				EndLocation,
				ECollisionChannel::ECC_Visibility
			);

			Scan.Add(CurrentRotation.RotateVector(Hit.Location - CurrentLocation));
		}

	}


	//std::ofstream* file = nullptr;
	//if (isFirst)
	//{
	//	file = new std::ofstream("D:/LidarScan.csv");
	//	*file << "x, y, z" << std::endl;
	//}

	//	
	//	if (isFirst)
	//	{
	//		*file << Hit.Location.X << "," << Hit.Location.Y << "," << Hit.Location.Z << std::endl;
	//	}
	//}
	//
	//UE_LOG(LogTemp, Warning, TEXT("Done %d ray-casts"), Scan.Num());
	//UE_LOG(LogTemp, Warning, TEXT("Example: %f"), Scan[7].X);

	//if (file)
	//{
	//	file->close();
	//	delete file;
	//}

	isFirst = false;
}

