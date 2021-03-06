// Fill out your copyright notice in the Description page of Project Settings.

#include "LidarComponent.h"
#include <Engine/World.h>
#include "SensorManager.h"
#include "TazioVehicle.h"
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

	Owner = Cast<ATazioVehicle>(GetOwner());
	World = GetWorld();

	check(Owner);
	check(World);
}


// Called every frame
void ULidarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// FIXME it was crashing
	//return;

	//UE_LOG(LogTemp, Warning, TEXT("FPS: %f"), 1.f / DeltaTime);

	//int TotalPoints = PointsPerLayer * Layers* NLidars;
	//TotalPoints = 72;

	// Request lidar scan depending on sensor frequency
	if (World->GetTimeSeconds() >= InitTime + LidarSimulated*1/LidarFrequency)
	{
		//ULidarMessage* Scan = NewObject<ULidarMessage>();
		FLidarMessage Scan;

		//Scan.Empty();
		Scan.Points.reserve(LidarPoses.Num()*VerLayers*HorPoints);
		
		/*if (!isFirst)
			UE_LOG(LogTemp, Warning, TEXT("Lidar frequency: %f"), 1.f / (World->GetTimeSeconds() - LastLidarScanTime));*/

			// Update last scan time
		LastLidarScanTime = World->GetTimeSeconds();

		Scan.timestamp = LastLidarScanTime;

		FVector CurrentLocation = Owner->GetActorLocation();
		//FVector CurrentLocation = GetLidarLocation();
		FRotator CurrentRotation = Owner->GetActorRotation();

		/*TSet<UActorComponent*> components = Owner->GetComponents();
		for (auto& comp : components)
		{
			if (comp->GetName() == "InternalCameraBase")
				UE_LOG(LogTemp, Warning, TEXT("Rotation: %s"), *(Cast<USceneComponent>(comp)->GetComponentRotation().ToString()));

		}*/

		FHitResult Hit;

		int count = 0;

		for (int lidar = 0; lidar < LidarPoses.Num(); lidar++)
		{
			for (int layer = 0; layer < VerLayers; layer++)
			{
				for (int point = 0; point < HorPoints; point++)
				{
					FVector CurrentLidarLocation = CurrentRotation.RotateVector(LidarPoses[lidar].GetLocation());
					FRotator CurrentLidarRotation = LidarPoses[lidar].GetRotation().Rotator();

					FQuat CurrentQuat(CurrentRotation);
					FQuat LayerQuat(FRotator(FirstLidarVerOffset + layer * VerResolution, 0.f, 0.f));
					//FQuat HorScanQuat(FRotator(0.f, FirstLidarHorOffset + lidar * HorSeparation + point * HorResolution, 0.f));
					FQuat HorScanQuat(FRotator(0.f, CurrentLidarRotation.Yaw - HorPoints*HorResolution/2.0 + point * HorResolution, 0.f));

					FRotator ThisRotation = (CurrentQuat*HorScanQuat*LayerQuat).Rotator();
					FVector EndLocation = CurrentLocation + LidarRange * (ThisRotation.Vector());

					bool isValidHit = World->LineTraceSingleByChannel(
						Hit,
						CurrentLocation + CurrentLidarLocation,
						EndLocation,
						ECollisionChannel::ECC_Visibility
					);

					if (isValidHit)
					{
						FVector SensorPoint = CurrentRotation.UnrotateVector(Hit.Location - CurrentLocation);

						//// Transform to meters and NWU frame --- for my pawn
						//Scan.PointsX.push_back(SensorPoint.X*0.01); 
						//Scan.PointsY.push_back(-SensorPoint.Y*0.01); 
						//Scan.PointsZ.push_back(SensorPoint.Z*0.01); 

						Scan.Points.push_back(Point3D(SensorPoint.X*0.01, -SensorPoint.Y*0.01, SensorPoint.Z*0.01));

						//// Transform to meters and NWU frame --- for advanced vehicle
						//Scan.PointsX.push_back(-SensorPoint.X*0.01);
						//Scan.PointsY.push_back(SensorPoint.Y*0.01);
						//Scan.PointsZ.push_back(-SensorPoint.Z*0.01);

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

							DrawDebugLine(GetWorld(), CurrentLocation + CurrentLidarLocation, Hit.Location, DebugColor, false, -1.f);

						}
					}
					else
					{
						/*Scan.PointsX[count] = -1;
						Scan.PointsY[count] = -1;
						Scan.PointsZ[count] = -1;*/
					}

					count++;
				}
			}
		}

		LidarSimulated++;

		// Delegate broadcast
		Scan.Points.shrink_to_fit();
		
		Owner->GetSensorManager()->SendLidarMessage(Scan);
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
	return (Owner->GetActorLocation() + FVector(400.f, 0.f, 50.0));
}
