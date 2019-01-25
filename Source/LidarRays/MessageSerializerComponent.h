// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "simulink_interface/archive.hpp"
#include "MessageSerializerComponent.generated.h"

class ULidarMessage;
class UIMUMessage;

UCLASS(ClassGroup = (Sensors), meta = (BlueprintSpawnableComponent))
class LIDARRAYS_API UMessageSerializerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMessageSerializerComponent();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = Sensors)
		TArray<uint8> SerializeLidarMessage(ULidarMessage* msg, bool Simulink = false);

	UFUNCTION(BlueprintCallable, Category = Sensors)
		TArray<uint8> SerializeIMUMessage(UIMUMessage* msg, bool Simulink = false);

	UFUNCTION(BlueprintCallable, Category = Sensors)
		TArray<uint8> SerializeOdometryMessage(UOdometryMessage* msg, bool Simulink = false);


	template <class MessageType>
		TArray<uint8> SerializeMessage(MessageType* msg, bool Simulink);

	template <class MessageType>
		MessageType* DeserializeMessage(TArray<uint8> bytes, bool Simulink);

	UFUNCTION(BlueprintCallable, Category = Controller)
		UControlMessage* DeserializeControlMessage(TArray<uint8> bytes, bool Simulink = true);



};

template <class MessageType>
TArray<uint8> UMessageSerializerComponent::SerializeMessage(MessageType * msg, bool Simulink)
{
	if (msg == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Message serializer has received null message"));
		return TArray<uint8>();
	}

	std::ostringstream oss;

	if (Simulink)
	{
		simulink::SimulinkOutputArchive oa(oss);
		oa << (*msg);
	}
	else
	{
		cereal::PortableBinaryOutputArchive oa(oss);
		oa << (*msg);
	}

	oss.flush();

	oss.seekp(0, std::ios::end);
	int32 bytes_size = oss.tellp();

	oss.seekp(0, std::ios::beg);

	std::string string_data = oss.str();

	TArray<uint8> serialized;

	long i = 0;
	for (; i < bytes_size; i++)
	{
		serialized.Add(string_data[i]);
	}


	return serialized;
}

template <class MessageType>
MessageType * UMessageSerializerComponent::DeserializeMessage(TArray<uint8> bytes, bool Simulink)
{
	//UE_LOG(LogTemp, Warning, TEXT("MessageSerializer: Going to deserialize %i bytes"), ControlMessage.Num());

	std::istringstream iss;
	std::string bytes_str;

	// TODO efficiency issue, bytes are copied into a string
	for (int i = 0; i < bytes.Num(); i++)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Deserialize byte %i: %s"), i, *BytesToHex(&ControlMessage[i], 1));
		bytes_str.push_back((char)bytes[i]);
	}

	iss.str(bytes_str);

	//UE_LOG(LogTemp, Warning, TEXT("MessageSerializer: String is %i char long"), ControlMessage.Num());

	MessageType* msg = NewObject<MessageType>();

	if (Simulink)
	{
		simulink::SimulinkInputArchive ia(iss);

		// Deserialize
		try
		{
			ia >> (*msg);
		}
		catch (std::exception&)
		{
			return nullptr;
		}
	}
	else
	{
		cereal::PortableBinaryInputArchive ia(iss);

		// Deserialize
		try
		{
			ia >> (*msg);
		}
		catch (std::exception&)
		{
			return nullptr;
		}
	}

	return msg;
}