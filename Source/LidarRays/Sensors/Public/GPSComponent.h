#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <random>
#include "OdometryMessage.h"
#include "GPSComponent.generated.h"

class AActor;
class UWorld;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGPSAvailableDelegate, const FOdometryMessage&, GPSData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGPSTruthAvailableDelegate, const FOdometryMessage&, GPSTruthData);

UCLASS(ClassGroup = (Sensors), meta = (BlueprintSpawnableComponent))
class LIDARRAYS_API UGPSComponent : public UActorComponent
{
	GENERATED_BODY()

	AActor* Owner = nullptr;
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

	// deg
	UPROPERTY(EditAnywhere, Category = Noise)
	float YawStdDev = 1.f;

	std::normal_distribution<float> PositionRandomNoise;
	std::normal_distribution<float> YawRandomNoise;

public:

	UGPSComponent();
	~UGPSComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable, Category = Sensors)
		FGPSAvailableDelegate OnGPSAvailable;

	UPROPERTY(BlueprintAssignable, Category = Sensors)
		FGPSTruthAvailableDelegate OnGPSTruthAvailable;

private:
	void ToggleGPS();

};
