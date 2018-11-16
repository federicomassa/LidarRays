#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoseStampedMessage.h"
#include "MessageWrapper.h"
#include "GPSComponent.generated.h"

class AActor;
class UWorld;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGPSAvailableDelegate, UOutgoingMessage*, GPSData);


UCLASS(ClassGroup = (Sensors), meta = (BlueprintSpawnableComponent))
class LIDARRAYS_API UGPSComponent : public UActorComponent
{
	GENERATED_BODY()

	AActor* Owner = nullptr;
	UWorld* World = nullptr;
	USkeletalMeshComponent* Mesh;;

	FVector InitLocation;
	FRotator InitRotation;
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

};