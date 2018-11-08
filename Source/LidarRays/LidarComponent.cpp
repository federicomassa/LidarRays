// Fill out your copyright notice in the Description page of Project Settings.

#include "LidarComponent.h"
#include <Engine/World.h>
#include <GameFramework/Actor.h>
#include <DrawDebugHelpers.h>


// Sets default values for this component's properties
ULidarComponent::ULidarComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}

ULidarComponent::~ULidarComponent()
{
}


// Called when the game starts
void ULidarComponent::BeginPlay()
{
	Super::BeginPlay();

	InitTime = GetWorld()->GetTimeSeconds();

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

	//Scan = NewObject<ULidarMessage>();

}


// Called every frame
void ULidarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//UE_LOG(LogTemp, Warning, TEXT("FPS: %f"), 1.f / DeltaTime);

	//int TotalPoints = PointsPerLayer * Layers* NLidars;
	//TotalPoints = 72;

	// Request lidar scan depending on sensor frequency
	if (World->GetTimeSeconds() >= InitTime + LidarSimulated*1/LidarFrequency)
	{
		ULidarMessage* Scan = NewObject<ULidarMessage>();

		Scan->Empty();

		/*if (!isFirst)
			UE_LOG(LogTemp, Warning, TEXT("Lidar frequency: %f"), 1.f / (World->GetTimeSeconds() - LastLidarScanTime));*/

			// Update last scan time
		LastLidarScanTime = World->GetTimeSeconds();

		Scan->timestamp = LastLidarScanTime;

		//FVector CurrentLocation = Owner->GetActorLocation();
		FVector CurrentLocation = GetLidarLocation();
		FRotator CurrentRotation = Owner->GetActorRotation();

		/*TSet<UActorComponent*> components = Owner->GetComponents();
		for (auto& comp : components)
		{
			if (comp->GetName() == "InternalCameraBase")
				UE_LOG(LogTemp, Warning, TEXT("Rotation: %s"), *(Cast<USceneComponent>(comp)->GetComponentRotation().ToString()));

		}*/

		FHitResult Hit;

		for (int lidar = 0; lidar < NumLidars; lidar++)
		{
			for (int layer = 0; layer < VerLayers; layer++)
			{
				for (int point = 0; point < HorPoints; point++)
				{
					FQuat CurrentQuat(CurrentRotation);
					FQuat LayerQuat(FRotator(FirstLidarVerOffset + layer * VerResolution, 0.f, 0.f));
					FQuat HorScanQuat(FRotator(0.f, FirstLidarHorOffset + lidar * HorSeparation + point * HorResolution, 0.f));

					FRotator ThisRotation = (CurrentQuat*HorScanQuat*LayerQuat).Rotator();
					FVector EndLocation = CurrentLocation + LidarRange * (ThisRotation.Vector());

					bool isValidHit = World->LineTraceSingleByChannel(
						Hit,
						CurrentLocation,
						EndLocation,
						ECollisionChannel::ECC_Visibility
					);

					if (isValidHit)
					{
						FVector SensorPoint = CurrentRotation.UnrotateVector(Hit.Location - CurrentLocation);

						// Transform to meters and ENU frame --- for my pawn
						Scan->PointsX.push_back(SensorPoint.Y*0.01); 
						Scan->PointsY.push_back(SensorPoint.X*0.01); 
						Scan->PointsZ.push_back(-SensorPoint.Z*0.01); 

						//// Transform to meters and ENU frame --- for advanced vehicle
						//Scan->PointsX.push_back(-SensorPoint.X*0.01);
						//Scan->PointsY.push_back(SensorPoint.Y*0.01);
						//Scan->PointsZ.push_back(-SensorPoint.Z*0.01);


						if (DebugLines)
						{
							FColor DebugColor;
							if (layer == 0)
								DebugColor = FColor::Red;
							else if (layer == 1)
								DebugColor = FColor::Green;
							else if (layer == 2)
								DebugColor = FColor::Blue;
							else if (layer == 3)
								DebugColor = FColor::Yellow;
							else
								DebugColor = FColor::Black;

							DrawDebugLine(GetWorld(), CurrentLocation, Hit.Location, DebugColor, false, -1.f);
						}
					}
				}
			}
		}

		LidarSimulated++;

		// Delegate broadcast
		OnLidarAvailable.Broadcast(Scan);
		//isFirst = false;
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

}

FVector ULidarComponent::GetLidarLocation() const
{
	return (Owner->GetActorLocation() + FVector(0.f, 0.f, ZOffset));
}
