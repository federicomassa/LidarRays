// Fill out your copyright notice in the Description page of Project Settings.

#include "TazioGameMode.h"
#include <Engine/World.h>
#include <Kismet/GameplayStatics.h>
#include "TazioGameInstance.h"
#include <GameFramework/PlayerStart.h>
#include <GameFramework/Pawn.h>
#include <Internationalization/Regex.h>

void ATazioGameMode::BeginPlay()
{
	Super::BeginPlay();
}

APawn* ATazioGameMode::SpawnContestants(UClass* CharacterClass)
{
	UTazioGameInstance* GameInstance = Cast<UTazioGameInstance>(GetGameInstance());
	check(GameInstance);

	APawn* Character = nullptr;

	for (int ContestantsCount = 0; ContestantsCount < GameInstance->Contestants; ContestantsCount++)
	{
		FActorSpawnParameters params;
		params.Name = FName(*(FString("Player_") + FString::FromInt(ContestantsCount)));

		FVector Location(0.0, 0.0, 150 + ContestantsCount*200);
		FRotator Rotation(0.0, 0.0, 0.0);

		APawn* Contestant = Cast<APawn>(GetWorld()->SpawnActor(CharacterClass, &Location, &Rotation, params));

		if (ContestantsCount == 0)
			Character = Contestant;		
	}

	return Character;
}