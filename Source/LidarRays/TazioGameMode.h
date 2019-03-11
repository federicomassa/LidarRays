// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "RaceControl.h"
#include "TazioGameMode.generated.h"

/**
 * 
 */


UCLASS()
class LIDARRAYS_API ATazioGameMode : public AGameMode
{
	GENERATED_BODY()

private:
	RaceControl raceControl;
	TArray<AActor*> contestants;
public:
	void BeginPlay() override;
	void Tick(float DeltaTime) override;

	// Returns pointer to character
	UFUNCTION(BlueprintCallable)
	APawn* SpawnContestants(UClass* CharacterClass, UClass* OpponentsClass);
};


