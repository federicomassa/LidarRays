#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IMUMessage.h"
#include "IMUComponent.generated.h"

class AActor;
class UWorld;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIMUAvailableDelegate, const FIMUMessage&, IMUData);


UCLASS(ClassGroup = (Sensors), meta = (BlueprintSpawnableComponent))
class LIDARRAYS_API UIMUComponent : public UActorComponent
{
	GENERATED_BODY()

	AActor* Owner = nullptr;
	UWorld* World = nullptr;
	USkeletalMeshComponent* Mesh;;



	float InitTime;

	float LastVx;
	float LastVy;
	float LastVz;
	float LastPitch;
	float LastYaw;
	float LastRoll;

public:

	UIMUComponent();
	~UIMUComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable, Category = IMU)
	FIMUAvailableDelegate OnIMUAvailable;

};