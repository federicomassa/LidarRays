// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "UDPSender.h"
#include <GameFramework/Actor.h>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>

// endian conversion
#include <boost/endian/conversion.hpp>

AUDPSender::AUDPSender()
{
	SenderSocket = NULL;
	ShowOnScreenDebugMessages = true;
}

AUDPSender::~AUDPSender()
{
	UE_LOG(LogTemp, Warning, TEXT("!!!!!!!!!!!UDP SENDER DESTRUCTOR!!!!!!!!!!!!!!"));
}

bool AUDPSender::SendData(TArray<uint8> Data)
{
	// ======= FIXME DEBUG ========
	if (!isFirst) return false;
	// ============================

	if (!SenderSocket)
	{
		ScreenMsg("No sender socket"); 
		return false; 
	} 
	
	int32 BytesSent = 0;

	//UE_LOG(LogTemp, Warning, TEXT("Sender data to send: %d"), Data.Num());
	//SenderSocket->SendTo(Writer.GetData(), Writer.Num(), BytesSent, *RemoteAddr);
	uint8* bytes = new uint8[Data.Num()];

	for (int i = 0; i < Data.Num(); i++)
	{
		bytes[i] = Data[i];
	}

	//float x = 5.67;
	//SenderSocket->SendTo(bytes, sizeof(x), BytesSent, *RemoteAddr);

	// ========= First message with buffer size
	size_t buffer_size = Data.Num();
	//UE_LOG(LogTemp, Warning, TEXT("Buffer size %d"), buffer_size);
	//// Serialize number indicating size of data buffer
	//std::ostringstream oss;
	//cereal::PortableBinaryOutputArchive oa(oss);
	//oa << buffer_size;
	//oss.flush();

	//std::string buffer_size_string = oss.str();
	//int buffer_size_size = buffer_size_string.size();

	//uint8* buffer_size_bytes = new uint8[buffer_size_size];
	//for (int i = 0; i < buffer_size_size; i++)
	//	buffer_size_bytes[i] = buffer_size_string[i];

	int32 SizeMessageBytesSent = 0;
	size_t buffer_size_size = sizeof(buffer_size);

	size_t buffer_size_bigendian = boost::endian::native_to_big(buffer_size);


	// ============= TEST =============
	//size_t buffer_size_bigendian = boost::endian::native_to_big(test_message++);

	// ================================
	//UE_LOG(LogTemp, Warning, TEXT("Big endian!"));
	//SenderSocket->SendTo(buffer_size_bytes, buffer_size_size, SizeMessageBytesSent, *RemoteAddr);

	if (!isUDP)
	{
		SenderSocket->SendTo((uint8*)&buffer_size_bigendian, sizeof(buffer_size_bigendian), SizeMessageBytesSent, *RemoteAddr);
		if (SizeMessageBytesSent <= 0)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Hey wtf"));
			const FString Str = "First message: socket is valid but the receiver received 0 bytes, make sure it is listening properly!";
			UE_LOG(LogTemp, Error, TEXT("%s"), *Str);
			//ScreenMsg(Str);
			//UE_LOG(LogTemp, Warning, TEXT("First message: sent %d/%d bytes"), SizeMessageBytesSent, buffer_size_size);
			return false;
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("First message: sent %d/%d bytes"), SizeMessageBytesSent, buffer_size_size);

	//SenderSocket->SendTo(message_size.bytes, sizeof(buffer_size), SizeMessageBytesSent, *RemoteAddr);

	// =========  Second message with actual data
	SenderSocket->SendTo(bytes, Data.Num(), BytesSent, *RemoteAddr);
	if (BytesSent <= 0) 
	{ 
		const FString Str = "Lidar message:	Socket is valid but the receiver received 0 bytes, make sure it is listening properly!"; 
		UE_LOG(LogTemp, Error, TEXT("%s"), *Str);
		//ScreenMsg(Str); 
		return false; 
	}

	//ScreenMsg("Should send ", Data.Num());
	//ScreenMsg("Sent: ", BytesSent);

	delete[] bytes;
	//delete[] buffer_size_bytes;

	//UE_LOG(LogTemp, Warning, TEXT("End TCP Send at time: %f"), GetWorld()->GetTimeSeconds());
	//isFirst = false;

	return true;
}

bool AUDPSender::Start(const FString & YourChosenSocketName, const FString & TheIP, const int32 ThePort, bool UDP)
{
	//Create Remote Address. 
	RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	isUDP = UDP;

	bool bIsValid; 
	RemoteAddr->SetIp(*TheIP, bIsValid);
	RemoteAddr->SetPort(ThePort);

	if (!bIsValid) 
	{ 
		ScreenMsg("UDP Sender >> IP address was not valid!", TheIP); 
		return false;
	}

	if (isUDP)
	{
		SenderSocket = FUdpSocketBuilder(*YourChosenSocketName).AsReusable().WithBroadcast();
		check(SenderSocket->GetSocketType() == SOCKTYPE_Datagram);

	}
	else
	{
		SenderSocket = FTcpSocketBuilder(*YourChosenSocketName)
			.AsReusable();
		check(SenderSocket->GetSocketType() == SOCKTYPE_Streaming);
	}

	////Set Send Buffer Size
	/*int32 SendSize = 2*1024*1024; 
	SenderSocket->SetSendBufferSize(SendSize,SendSize);
	*/

	//UE_LOG(LogTemp, Warning, TEXT("Sender socket buff size: %d"), SendSize);

	//SenderSocket->SetReceiveBufferSize(SendSize, SendSize);

	bool connected = SenderSocket->Connect(*RemoteAddr);

	if (connected)
	{
		UE_LOG(LogTemp, Log, TEXT("\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
		UE_LOG(LogTemp, Log, TEXT("****UDP**** Sender Initialized Successfully!!!"));
		UE_LOG(LogTemp, Log, TEXT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n"));
		return connected;
	}

	return connected;
}

void AUDPSender::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (SenderSocket) 
	{ 
		SenderSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SenderSocket); 
	}


}
