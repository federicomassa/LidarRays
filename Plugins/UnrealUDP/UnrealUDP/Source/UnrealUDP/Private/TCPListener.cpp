// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TCPListener.h"
#include "Engine.h"

//bool ATCPListener::StartTCPReceiver(const FString& YourSocketName, const FString& TheIP, const int32 ThePort)
//{
//	ListenerSocket = CreateTCPConnectionListener(YourSocketName, TheIP, ThePort);
//
//	if (!ListenerSocket)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("StartTCPReceiver >> Listener socket could not be created: %s, %d"), *TheIP, ThePort));
//		return false;
//	}
//
//	//GetWorldTimerManager().SetTimer(this, &ATCPListener::TCPConnectionListener, 0.01, true);
//	
//	return true;
//}
//
//bool ATCPListener::FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4])
//{
//	// IP formatting
//	TheIP.Replace(TEXT(" "), TEXT(""));
//
//	TArray<FString> Parts;
//	TheIP.ParseIntoArray(&Parts, TEXT("."), true);
//	if (Parts.Num() != 4)
//		return false;
//
//	for (int32 i = 0; i < 4; ++i)
//	{
//		Out[i] = FCString::Atoi(*Parts[i]);
//	}
//		
//	return true;
//}
//
//FSocket* ATCPListener::CreateTCPConnectionListener(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort, const int32 ReceiveBufferSize)
//{
//	uint8 IP4Nums[4];
//	if (!FormatIP4ToNumber(TheIP, IP4Nums))
//	{
//		UE_LOG(LogTemp, Error, TEXT("Invalid IP! Expecting 4 parts separated by ."));
//		return false;
//	}
//
//	FIPv4Endpoint Endpoint(FIPv4Address(IP4Nums[0], IP4Nums[1], IP4Nums[2], IP4Nums[3]), ThePort);
//	FSocket* ListenSocket = FTcpSocketBuilder(*YourChosenSocketName)
//		.AsReusable()
//		.BoundToEndpoint(Endpoint)
//		.Listening(8);
//
//	// Set buffer size
//	int32 NewSize = 0;
//	ListenSocket->SetReceiveBufferSize(ReceiveBufferSize, NewSize);
//
//	return ListenSocket;
//}
//
//void ATCPListener::TCPConnectionListener()
//{
//	if (!ListenerSocket) return;
//
//	// Remote address
//	TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
//	bool Pending;
//
//	// handle incoming connections
//	if (ListenerSocket->HasPendingConnection(Pending) && Pending)
//	{
//		// Already have a connection? Destroy previous
//		if (ConnectionSocket)
//		{
//			ConnectionSocket->Close();
//			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
//		}
//
//		// New connection receive!
//		ConnectionSocket = ListenerSocket->Accept(*RemoteAddress, TEXT("TCP Received Socket connection"));
//		if (ConnectionSocket != NULL)
//		{
//			// Global cache of current Remote Address
//			RemoteAddressForConnection = FIPv4Endpoint(RemoteAddress);
//			GetWorldTimerManager().SetTimer(this, &ATCPListener::TCPSocketListener, 0.01, true);
//		}
//	}
//}
//
//FString ATCPListener::StringFromBinaryArray(const TArray<uint8>& BinaryArray)
//{
//	BinaryArray.Add(0);
//	return FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(BinaryArray.GetData())));
//}
//
//void ATCPListener::TCPSocketListener()
//{
//	if (!ConnectionSocket) return;
//
//	TArray<uint8> ReceivedData;
//	uint32 Size;
//
//	while (ConnectionSocket->HasPendingData(Size))
//	{
//		ReceivedData.Init(FMath::Min(Size, 65507u));
//		int32 Read = 0;
//		ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
//
//		if (ReceivedData.Num() <= 0)
//		{
//			// No data received
//			return;
//		}
//
//		UE_LOG(LogTemp, Warning, TEXT("Total Data read: %d"), ReceivedData.Num());
//
//		const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
//	}
//}