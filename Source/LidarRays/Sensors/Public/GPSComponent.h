#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoseStampedMessage.h"
#include "GPSComponent.generated.h"

class AActor;
class UWorld;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGPSAvailableDelegate, UOdometryMessage*, GPSData);


UCLASS(ClassGroup = (Sensors), meta = (BlueprintSpawnableComponent))
class LIDARRAYS_API UGPSComponent : public UActorComponent
{
	GENERATED_BODY()

	AActor* Owner = nullptr;
	UWorld* World = nullptr;
	USkeletalMeshComponent* Mesh;;



	float InitTime;

	TArray<UPoseStampedMessage*> PastPoses;
	float LastVx;
	float LastVy;
	float LastVz;

public:

	UGPSComponent();
	~UGPSComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable, Category = GPS)
	FGPSAvailableDelegate OnGPSAvailable;


	FVector GetGPSOrientation() const;
	FVector GetGPSAngularVelocity() const;
	FVector GetGPSLinearAcceleration() const;

};