// Fill out your copyright notice in the Description page of Project Settings.

#include "LidarComponent.h"
#include "MyPawn.h"
// #include "FSensorWorker.h"
#include <Engine/World.h>
#include <PlatformFile.h>
#include <PlatformFilemanager.h>
#include <FileHelper.h>
#include <fstream>


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

	Owner = Cast<AMyPawn>(GetOwner());
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
	UE_LOG(LogTemp, Warning, TEXT("FPS: %f"), 1.f / DeltaTime);


	int PointsPerLayer = 600;
	int Layers = 4;
	int NLidars = 4;

	int TotalPoints = PointsPerLayer * Layers* NLidars;
	TotalPoints = 100;

	// Request lidar scan depending on sensor frequency
	if (isFirst || World->GetTimeSeconds() - LastLidarScanTime > 1.f/LidarFrequency)
	{
		if (SensorWorker)
		{
			if (!isFirst && !SensorWorker->IsThreadFinished())
			{
				UE_LOG(LogTemp, Error, TEXT("Lidar simulation was not able to complete with required frequency"));
				return;
			}

		SensorWorker->Shutdown();
		Scan.Empty();
	}

		
		SensorWorker = FSensorWorker::JoyInit(Scan, TotalPoints, World->GetFirstPlayerController(), GetOwner(), GetWorld());

		

		if (!isFirst)
			UE_LOG(LogTemp, Warning, TEXT("Lidar frequency: %f"), 1.f / (World->GetTimeSeconds() - LastLidarScanTime));

		// Update last scan time
		LastLidarScanTime = World->GetTimeSeconds();

		//// Create and run thread
		//SensorWorker->Init();
		//SensorWorker->Run();
	}


	//std::ofstream* file = nullptr;
	//if (isFirst)
	//{
	//	file = new std::ofstream("D:/LidarScan.csv");
	//	*file << "x, y, z" << std::endl;
	//}

	//for (int i = 0; i < TotalPoints; i++)
	//{
	//	FRotator ThisRotation = CurrentRotation + FRotator(0, i*DeltaYaw, 0);
	//	FVector EndLocation = CurrentLocation + LidarRange * (ThisRotation.Vector());

	//	World->LineTraceSingleByChannel(
	//		Hit,
	//		CurrentLocation,
	//		EndLocation,
	//		ECollisionChannel::ECC_Visibility
	//	);

	//	Scan.Add(Hit.Location);
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

