// Fill out your copyright notice in the Description page of Project Settings.

#include "MessageSerializerComponent.h"
#include <boost/archive/binary_oarchive.hpp>
#include <sstream>
#include "LidarMessage.h"


// Sets default values for this component's properties
UMessageSerializerComponent::UMessageSerializerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}


// Called when the game starts
void UMessageSerializerComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UMessageSerializerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

TArray<uint8> UMessageSerializerComponent::SerializeLidarMessage(ULidarMessage * Scan)
{
	if (Scan == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Message serializer has received null message"));
		return TArray<uint8>();
	}

	std::stringbuf buffer;

	boost::archive::binary_oarchive oa(buffer);
	oa << *Scan;

	TArray<uint8> serialized;
	for (int i = 0; i < buffer.in_avail(); i++)
		serialized.Add(buffer.sbumpc());

	return serialized;
	/*MyData receivedData;
	boost::archive::binary_iarchive ia(buffer);
	ia >> receivedData;*/
}

