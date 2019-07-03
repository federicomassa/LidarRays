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
