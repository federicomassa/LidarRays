#include "IMUComponent.h"


UIMUComponent::UIMUComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

UIMUComponent::~UIMUComponent()
{
}

void UIMUComponent::BeginPlay()
{
	Super::BeginPlay();

	InitTime = GetWorld()->GetTimeSeconds();

	Owner = GetOwner();
	World = GetWorld();

	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("IMU component has no owner"));
	}

	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("IMU component has no world"));
	}
}

void UIMUComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}