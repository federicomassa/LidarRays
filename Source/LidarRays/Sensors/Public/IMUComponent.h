#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IMUComponent.generated.h"

class AActor;
class UWorld;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIMUAvailableDelegate, UIMUMessage*, IMUSample);


UCLASS(ClassGroup = (Sensors), meta = (BlueprintSpawnableComponent))
class LIDARRAYS_API UIMUComponent : public UActorComponent
{
	GENERATED_BODY()

	AActor* Owner = nullptr;
	UWorld* World = nullptr;

public:

	UIMUComponent();
	~UIMUComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorCompnentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable, Category = IMU)
	FIMUAvailableDelegate OnIMUAvailable;

	UPROPERTY(EditAnywhere, Category = IMU)
	bool DebugIMU = false;

	FVector GetIMUOrientation() const;
	FVector GetIMUAngularVelocity() const;
	FVector GetIMULinearAcceleration() const;

};