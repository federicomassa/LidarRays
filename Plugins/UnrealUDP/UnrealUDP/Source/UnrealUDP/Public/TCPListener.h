// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Networking.h"
#include "GameFramework/EngineMessage.h"
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "TCPListener.generated.h"

UCLASS()
class UNREALUDP_API ATCPListener : public AActor
{
	GENERATED_BODY()
	
public:
	//ATCPListener(const FObjectInitializer& ObjectInitializer);

private:
	bool isTCP;
	
	FTimerHandle ListenTimer;
	FTimerHandle NewConnectionTimer;
	FTimerHandle ConnectionTryTimer;


public:
	FIPv4Endpoint RemoteAddressForConnection;
	FSocket* ListenerSocket;
	FSocket* ConnectionSocket;

	UFUNCTION(BlueprintCallable, Category=TCPListener)
		bool StartTCPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort);

	FSocket* CreateTCPConnectionListener(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort, const int32 ReceiveBufferSize = 2 * 1024 * 1024);

	void TCPConnectionListener();
	void TCPSocketListener();

	// Format String IP4 to number array
	bool FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4]);

	FString StringFromBinaryArray(TArray<uint8>& BinaryArray);
};