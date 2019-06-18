// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TazioGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LIDARRAYS_API ATazioGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	void BeginPlay() override;

	// Returns pointer to character
	UFUNCTION(BlueprintCallable)
	APawn* SpawnContestants(UClass* CharacterClass);
};


