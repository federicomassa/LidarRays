// Fill out your copyright notice in the Description page of Project Settings.

#include "MessageSerializerComponent.h"
// Binary was abandoned because not portable

// FIXME remove this as soon as VS solves the bug: https://stackoverflow.com/questions/51863797/new-vs-c-update-is-making-boost-forward-declared-code-un-compilable-what-can
#pragma warning( disable : 4643 ) 


//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/archive/portable_oarchive.hpp>
//#include <boost/archive/text_oarchive.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>

#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>
#include <sstream>
#include <Containers/UnrealString.h>
#include "LidarMessage.h"
#include "IMUMessage.h"
#include "TwistMessage.h"


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

	//UE_LOG(LogTemp, Warning, TEXT("Serializing scan points: %d"), Scan->PointsX.size());

	//std::stringbuf buffer;
	std::ostringstream oss;
	
	//std::stringstream buffer;

	//cereal::PortableBinaryOutputArchive oa(oss);
	cereal::BinaryOutputArchive oa(oss);


	//boost::archive::binary_oarchive oa(buffer);
	/*UTestMessage TestMessage;
	TestMessage.timestamp = 3.14159;
*/
	//eos::portable_oarchive oa(buffer);
	oa << *Scan;
	//oa << TestMessage;
	oss.flush();
	
	oss.seekp(0, std::ios::end);
	int32 bytes_size = oss.tellp();
	//UE_LOG(LogTemp, Warning, TEXT("Size with tellp: %d"), bytes_size);

	// Reset
	oss.seekp(0, std::ios::beg);

	std::string string_data = oss.str();
//	UE_LOG(LogTemp, Warning, TEXT("Serialized lidar message in %i bytes"), bytes_size);
	
	TArray<uint8> serialized;
	//UE_LOG(LogTemp, Warning, TEXT("String bytes: %d"), string_data.size());
	//string_data.size();

	long i = 0;
	for (; i < bytes_size; i++)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Byte %i: %s"), i, *BytesToHex((uint8*)&string_data[i], 1));
		serialized.Add(string_data[i]);
		//UE_LOG(LogTemp, Warning, TEXT("Processed %d out of %d"), i, buffer.in_avail());
	}

	//UE_LOG(LogTemp, Warning, TEXT("Added %d elements to serialized"), i);
	//UE_LOG(LogTemp, Warning, TEXT("Vector is %i elements long"), serialized.Num());

	//boost::archive::binary_iarchive ia(buffer);


	// =========== TEST TWIST MESSAGE --- REMOVE ==========
	//std::ostringstream twistoss;
	//cereal::BinaryOutputArchive twistoa(twistoss);

	//TwistMessage twist;
	//twist.Timestamp = 0.f;
	//twist.Linear.push_back(0.f);
	//twist.Linear.push_back(0.f);
	//twist.Linear.push_back(0.f);
	//twist.Angular.push_back(0.f);
	//twist.Angular.push_back(0.f);
	//twist.Angular.push_back(0.f);

	//twistoa << twist;

	//twistoss.flush();
	//std::string twist_string = twistoss.str();
	//int32 twist_bytes = twist_string.size();

	//for (int j = 0; j < twist_bytes; j++)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Twist Byte %i: %s"), j, *BytesToHex((uint8*)&twist_string[i], 1));
	//}


	// ====================================================
		
	//int counter = 0;
	//buffer.seekg(0, std::ios::beg);
	//while (!buffer.eof())
	//{
	//	//serialized.add((uint8)buffer.get());
	//	//UE_LOG(LogTemp, Warning, TEXT("Counter: %d"), counter);
	//	buffer.get();
	//	counter++;
	//	buffer.seekg(counter, std::ios::beg);

	//	if (counter > 1)
	//		break;
	//}

	//// ===== TEST CEREAL ====
	//std::istringstream iss;
	//iss.str(string_data);

	//cereal::PortableBinaryInputArchive ia(iss);
	//
	//ULidarMessage* InputScan = NewObject<ULidarMessage>();
	//ia >> *InputScan;

	//UE_LOG(LogTemp, Warning, TEXT("Input scan size: %d"), InputScan->PointsX.size());

	//for (int i = 0; i < InputScan->PointsX.size(); i++)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("X Difference %i-th point: %f"), i, InputScan->PointsX[i] - Scan->PointsX[i]);
	//	UE_LOG(LogTemp, Warning, TEXT("Y Difference %i-th point: %f"), i, InputScan->PointsY[i] - Scan->PointsY[i]);
	//	UE_LOG(LogTemp, Warning, TEXT("Z Difference %i-th point: %f"), i, InputScan->PointsZ[i] - Scan->PointsZ[i]);
	//}

	// ======================

	/*buffer.seekg(0, std::ios::end);
	int buffer_size = buffer.tellg();

	if (counter != serialized.Num())
		UE_LOG(LogTemp, Warning, TEXT("Buffer length does not correspond to output data size"));

	buffer.seekg(0, std::ios::beg);
	
	int a = buffer.get();*/

	return serialized;
	/*MyData receivedData;
	boost::archive::binary_iarchive ia(buffer);
	ia >> receivedData;*/
}

TArray<uint8> UMessageSerializerComponent::SerializeIMUMessage(UIMUMessage * Data)
{
	if (Data == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Message serializer has received null message"));
		return TArray<uint8>();
	}

	std::ostringstream oss;

	cereal::BinaryOutputArchive oa(oss);

	oa << *Data;
	//oa << TestMessage;
	oss.flush();

	oss.seekp(0, std::ios::end);
	int32 bytes_size = oss.tellp();
	//UE_LOG(LogTemp, Warning, TEXT("Size with tellp: %d"), bytes_size);

	// Reset
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

TArray<uint8> UMessageSerializerComponent::SerializeMessage(UOutgoingMessage * Data)
{
	if (Data == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Message serializer has received null message"));
		return TArray<uint8>();
	}
	else if (Data->message == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Message serializer has received message containing null data"));
		return TArray<uint8>();
	}

	std::ostringstream oss;

	cereal::BinaryOutputArchive oa(oss);

	oa << (*Data->message);
	//oa << TestMessage;
	oss.flush();

	oss.seekp(0, std::ios::end);
	int32 bytes_size = oss.tellp();
	//UE_LOG(LogTemp, Warning, TEXT("Size with tellp: %d"), bytes_size);

	// Reset
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

UIncomingMessage * UMessageSerializerComponent::DeserializeControlMessage(TArray<uint8> ControlMessage)
{
	//UE_LOG(LogTemp, Warning, TEXT("MessageSerializer: Going to deserialize %i bytes"), ControlMessage.Num());

	std::istringstream iss;
	std::string msg;

	for (int i = 0; i < ControlMessage.Num(); i++)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Deserialize byte %i: %s"), i, *BytesToHex(&ControlMessage[i], 1));
		msg.push_back((char)ControlMessage[i]);
	}

	iss.str(msg);

	//UE_LOG(LogTemp, Warning, TEXT("MessageSerializer: String is %i char long"), ControlMessage.Num());


	cereal::BinaryInputArchive ia(iss);
	TwistMessage<cereal::BinaryInputArchive> Twist;

	// Deserialize
	ia(Twist);

	if (Twist.Linear.size() != 3 || Twist.Angular.size() != 3)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not deserialize twist message!!"));
		return nullptr;
	}

	UIncomingMessage* UTwist = NewObject<UIncomingMessage>();
	UTwist->message = new TwistMessage<cereal::BinaryInputArchive>(Twist);

	return UTwist;
}

