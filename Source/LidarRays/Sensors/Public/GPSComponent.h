#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <random>
#include "OdometryMessage.h"
#include <fstream>
#include "GPSComponent.generated.h"

class AActor;
class UWorld;
class ATazioVehicle;

UCLASS(ClassGroup = (Sensors), meta = (BlueprintSpawnableComponent))
class LIDARRAYS_API UGPSComponent : public UActorComponent
{
	GENERATED_BODY()

	ATazioVehicle* Owner = nullptr;
	UWorld* World = nullptr;
	USkeletalMeshComponent* Mesh;;

	FVector InitLocation;
	FRotator InitRotation;

	bool isActive = true;

	// Random generator
	std::random_device RandomDevice;
	std::mt19937 RandomGenerator;

	// meters
	UPROPERTY(EditAnywhere, Category = Noise)
	float PositionStdDev = 0.3f;

	UPROPERTY(EditAnywhere, Category = Noise)
	float PosXBias = 0.f;

	UPROPERTY(EditAnywhere, Category = Noise)
	float PosYBias = 0.f;

	// deg
	UPROPERTY(EditAnywhere, Category = Noise)
	float YawStdDev = 1.f;

	UPROPERTY(EditAnywhere, Category = Noise)
	float YawBias = 0.f;


	std::normal_distribution<float> PosXRandomNoise;
	std::normal_distribution<float> PosYRandomNoise;
	std::normal_distribution<float> YawRandomNoise;

	std::ofstream csv_file;

public:

	UGPSComponent();
	~UGPSComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void ToggleGPS();

};
