// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Definitions.h"
#include "Templates/Tuple.h"
#include "PlayerSettings.h"
#include "TazioGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class LIDARRAYS_API UTazioGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString LevelName = "";

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	EVehicleModelEnum VehicleModel = EVehicleModelEnum::VM_PhysX;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString LidarSendIP = "127.0.0.1";

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 LidarPort = 8890;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString GPSSendIP = "127.0.0.1";

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 GPSPort = 8891;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString GPSTruthSendIP = "127.0.0.1";

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 GPSTruthPort = 8893;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString IMUSendIP = "127.0.0.1";

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 IMUPort = 8892;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString ControlReceiveIP = "127.0.0.1";

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 ControlPort = 7790;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString OpponentStateSendIP = "127.0.0.1";

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 OpponentStatePort = 9000;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool isCommunicationUDP = true;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	int32 Contestants = 1;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString DumpTrajectory = "";
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString ReplayTrajectoryPath = "";

	// key is player index
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TMap<int, FPlayerSettings> PlayersSettings;
};
