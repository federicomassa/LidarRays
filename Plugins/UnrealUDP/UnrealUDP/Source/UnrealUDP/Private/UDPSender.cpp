// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "UDPSender.h"
#include <GameFramework/Actor.h>

AUDPSender::AUDPSender(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SenderSocket = NULL;
	ShowOnScreenDebugMessages = true;
}

bool AUDPSender::SendData(TArray<uint8> Data)
{
	if (!SenderSocket)
	{
		ScreenMsg("No sender socket"); 
		return false; 
	} 
	
	int32 BytesSent = 0;


	//SenderSocket->SendTo(Writer.GetData(), Writer.Num(), BytesSent, *RemoteAddr);
	uint8* bytes = new uint8[Data.Num()];
	for (int i = 0; i < Data.Num(); i++)
	{
		bytes[i] = Data[i];
	}

	//float x = 5.67;
	//SenderSocket->SendTo(bytes, sizeof(x), BytesSent, *RemoteAddr);
	
	SenderSocket->SendTo(bytes, Data.Num(), BytesSent, *RemoteAddr);
	if (BytesSent <= 0) { const FString Str = "Socket is valid but the receiver received 0 bytes, make sure it is listening properly!"; UE_LOG(LogTemp, Error, TEXT("%s"), *Str); ScreenMsg(Str); return false; }

	ScreenMsg("UDP~ Send Succcess! Bytes Sent = ", BytesSent);

	delete bytes;
	return true;
}

bool AUDPSender::Start(const FString & YourChosenSocketName, const FString & TheIP, const int32 ThePort, bool UDP)
{
	//Create Remote Address. 
	RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	bool bIsValid; 
	RemoteAddr->SetIp(*TheIP, bIsValid);
	RemoteAddr->SetPort(ThePort);

	if (!bIsValid) 
	{ 
		ScreenMsg("UDP Sender >> IP address was not valid!", TheIP); 
		return false;
	}

	SenderSocket = FUdpSocketBuilder(*YourChosenSocketName).AsReusable().WithBroadcast();

	check(SenderSocket->GetSocketType() == SOCKTYPE_Datagram);

	//Set Send Buffer Size
	int32 SendSize = 2*1024*1024; 
	SenderSocket->SetSendBufferSize(SendSize,SendSize);
	SenderSocket->SetReceiveBufferSize(SendSize, SendSize);

	UE_LOG(LogTemp, Log, TEXT("\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
	UE_LOG(LogTemp, Log, TEXT("****UDP**** Sender Initialized Successfully!!!"));
	UE_LOG(LogTemp, Log, TEXT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n"));

	return true;
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
