// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "UDPReceiver.h"
#include "AnyCustomData.h"
#include "Async.h"
#include <sstream>

// ==== DEBUG ======
//#include "ControlMessage.h"
//#include "simulink_interface/archive.hpp"
// =================

#include <string>
#include <GameFramework/Actor.h>

AUDPReceiver::AUDPReceiver()
{ 
	ListenSocket = NULL; 
}

AUDPReceiver::~AUDPReceiver()
{
	if (UDPReceiver)
		Finalize();
}

void AUDPReceiver::Finalize()
{
	delete UDPReceiver;
	UDPReceiver = nullptr;
	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	}
}

void AUDPReceiver::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Finalize();

	Super::EndPlay(EndPlayReason);
}

bool AUDPReceiver::Start(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort )
{

	FIPv4Address Addr;
	FIPv4Address::Parse(TheIP, Addr);

	//Create Socket
	FIPv4Endpoint Endpoint(Addr, ThePort); 

	//ListenSocket = FUdpSocketBuilder(*YourChosenSocketName).AsNonBlocking().AsReusable().BoundToEndpoint(Endpoint).WithReceiveBufferSize(BufferSize);
	ListenSocket = FUdpSocketBuilder(*YourChosenSocketName)
			.AsNonBlocking()
			.AsReusable()
			.BoundToPort(ThePort)
			.WithReceiveBufferSize(BufferSize).Build();
	
	if (!ListenSocket)
	{
		UE_LOG(LogTemp, Error, TEXT("Listen socket building failed"));
		return false;
	}
	
	ScreenMsg(FString::Printf(TEXT("Receiving from control board: %s:%i"), *TheIP, ThePort));


	FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
	UDPReceiver = new FUdpSocketReceiver(ListenSocket, ThreadWaitTime, TEXT("UDP RECEIVER"));
	UDPReceiver->OnDataReceived().BindUObject(this, &AUDPReceiver::Receive);
	UDPReceiver->Start();

	return true;
}

void AUDPReceiver::Receive(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt)
{
	// ======= https://github.com/getnamo/udp-ue4/blob/master/Source/UDPWrapper/Private/UDPComponent.cpp 

	TArray<uint8> Data;
	Data.AddUninitialized(ArrayReaderPtr->TotalSize());
	ArrayReaderPtr->Serialize(Data.GetData(), ArrayReaderPtr->TotalSize());

	//Pass the reference to be used on gamethread. This seems to be working better than sending controls to car in UDP thread
	AsyncTask(ENamedThreads::GameThread, [&, Data]()
	{
		OnDataReceived.Broadcast(Data);
	});
}