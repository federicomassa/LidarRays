// Fill out your copyright notice in the Description page of Project Settings.

#include "MessageSerializerComponent.h"
// Binary was abandoned because not portable

// FIXME remove this as soon as VS solves the bug: https://stackoverflow.com/questions/51863797/new-vs-c-update-is-making-boost-forward-declared-code-un-compilable-what-can
#pragma warning( disable : 4643 ) 

#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>

#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>
#include <sstream>
#include <Containers/UnrealString.h>
#include "LidarMessage.h"
#include "IMUMessage.h"
#include "OdometryMessage.h"
#include "ControlMessage.h"


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

TArray<uint8> UMessageSerializerComponent::SerializeOdometryMessage(const FOdometryMessage & msg, bool Simulink) const
{
	return SerializeMessage<FOdometryMessage>(msg, Simulink);
}

TArray<uint8> UMessageSerializerComponent::SerializeLidarMessage(const FLidarMessage & msg, bool Simulink) const
{
	return SerializeMessage<FLidarMessage>(msg, Simulink);
}

TArray<uint8> UMessageSerializerComponent::SerializeIMUMessage(const FIMUMessage & msg, bool Simulink) const
{
	return SerializeMessage<FIMUMessage>(msg, Simulink);
}

TArray<uint8> UMessageSerializerComponent::SerializeRaceControlMessage(const FRaceControlMessage & msg, bool Simulink) const
{
	return SerializeMessage<FRaceControlMessage>(msg, Simulink);
}

FControlMessage UMessageSerializerComponent::DeserializeControlMessage(const TArray<uint8>& bytes, bool Simulink) const
{
	return DeserializeMessage<FControlMessage>(bytes, Simulink);
}


