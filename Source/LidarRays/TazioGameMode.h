// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "EnvironmentParameters.h"
#include "TazioGameMode.generated.h"

/**
 * 
 */

class RaceControl;
class AUDPSender;
class ATazioVehicle;
class UTazioGameInstance;

UCLASS()
class LIDARRAYS_API ATazioGameMode : public AGameMode
{
	GENERATED_BODY()

private:
	RaceControl* raceControl;
	TArray<AActor*> contestants;

	int lastEgoZoneType = -1;

	// Manual race control mode
	//    if it is in manual mode
	bool manualRaceControl = false;
	//    if ego vehicle can overtake
	bool overtakeStatus = false;

	UPROPERTY()
	AUDPSender* RaceControlSender = nullptr;

	const ATazioVehicle* egoVehicle = nullptr;

	UTazioGameInstance* GameInstance = nullptr;
public:
	ATazioGameMode();
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void UpdateEnvironmentParameters(double time, EnvironmentParameters param);

	// Returns pointer to character
	UFUNCTION(BlueprintCallable)
	APawn* SpawnContestants(UClass* CharacterClass, UClass* OpponentsClass);

	void ToggleManualRaceControl();
	void ToggleOvertakeStatus();

	void SendRaceControl();
};