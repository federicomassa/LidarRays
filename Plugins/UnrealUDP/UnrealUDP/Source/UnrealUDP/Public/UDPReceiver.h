// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Networking.h"
#include "UdpSocketReceiver.h"
#include "UDPReceiver.generated.h"

struct FAnyCustomData;
class AActor;

UCLASS()
class AUDPReceiver : public AActor
{
	GENERATED_BODY()

public:
	AUDPReceiver(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintImplementableEvent)
	void BPEvent_DataReceived(const float& ReceivedData);
	//void BPEvent_DataReceived(const FAnyCustomData& ReceivedData);


public:
	FSocket* ListenSocket;
	FUdpSocketReceiver* UDPReceiver = nullptr;

	void Receive(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt);

	UFUNCTION(BlueprintCallable, Category = UDPReceiver)
	bool Start(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort);
	
	FORCEINLINE void ScreenMsg(const FString& Msg) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *Msg);
	}

	FORCEINLINE void ScreenMsg(const FString& Msg, const float Value)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s %f"), *Msg, Value));
	}

	FORCEINLINE void ScreenMsg(const FString& Msg, const FString& Msg2)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s %s"), *Msg, *Msg2));
	}

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};