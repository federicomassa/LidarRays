// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "UDPReceiver.h"
#include "AnyCustomData.h"
#include <GameFramework/Actor.h>

AUDPReceiver::AUDPReceiver(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) 
{ 
	ListenSocket = NULL; 
}

void AUDPReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	delete UDPReceiver;
	UDPReceiver = nullptr;
	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	}
}

bool AUDPReceiver::Start(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort )
{
	ScreenMsg("RECEIVER INIT");

	FIPv4Address Addr;
	FIPv4Address::Parse(TheIP, Addr);

	//Create Socket
	FIPv4Endpoint Endpoint(Addr, ThePort); 

	//BUFFER SIZE
	int32 BufferSize = 2*1024*1024;

	//ListenSocket = FUdpSocketBuilder(*YourChosenSocketName).AsNonBlocking().AsReusable().BoundToEndpoint(Endpoint).WithReceiveBufferSize(BufferSize);
	ListenSocket = FUdpSocketBuilder(*YourChosenSocketName).AsNonBlocking().AsReusable().BoundToPort(ThePort).WithReceiveBufferSize(BufferSize).Build();
	
	if (!ListenSocket)
	{
		UE_LOG(LogTemp, Error, TEXT("Listen socket building failed"));
		return false;
	}
	
	FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
	UDPReceiver = new FUdpSocketReceiver(ListenSocket, ThreadWaitTime, TEXT("UDP RECEIVER"));
	UDPReceiver->OnDataReceived().BindUObject(this, &AUDPReceiver::Receive);
	UDPReceiver->Start();

	return true;
}

void AUDPReceiver::Receive(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt)
{
	ScreenMsg("Received bytes", ArrayReaderPtr->Num());

	uint8* Data = ArrayReaderPtr->GetData();

	float* x = reinterpret_cast<float*>(Data);
	//FAnyCustomData Data;
	//*ArrayReaderPtr << Data;	//Now de-serializing! See AnyCustomData.h

	//BP Event 
	UE_LOG(LogTemp, Warning, TEXT("Received: %f"), *x);

	BPEvent_DataReceived(*x);
} 


