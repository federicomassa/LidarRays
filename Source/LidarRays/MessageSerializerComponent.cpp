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
#include <sstream>
#include <Containers/UnrealString.h>
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

	UE_LOG(LogTemp, Warning, TEXT("Serializing scan points: %d"), Scan->PointsX.size());

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

  //OLD VERSION FOR BINARY
	long i = 0;
	for (; i < bytes_size; i++)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Byte %i: %s"), i, *BytesToHex((uint8*)&string_data[i], 1));
		serialized.Add(string_data[i]);
		//UE_LOG(LogTemp, Warning, TEXT("Processed %d out of %d"), i, buffer.in_avail());
	}

	UE_LOG(LogTemp, Warning, TEXT("Added %d elements to serialized"), i);
	//UE_LOG(LogTemp, Warning, TEXT("Vector is %i elements long"), serialized.Num());

	//boost::archive::binary_iarchive ia(buffer);

		
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

