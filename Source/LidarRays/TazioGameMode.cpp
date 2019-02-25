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

APawn* ATazioGameMode::SpawnContestants(UClass* CharacterClass, UClass* OpponentsClass)
{
	// Find all PlayerStart objects in world
	TArray<AActor*> StartingPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), StartingPoints);

	UTazioGameInstance* GameInstance = Cast<UTazioGameInstance>(GetGameInstance());
	check(GameInstance);

	// A valid map must have at least as many player starts as number of contestants
	check(StartingPoints.Num() >= GameInstance->Contestants);

	FRegexPattern PlayerStartRegex("PlayerStart_([0-9]+)");

	APawn* Character = nullptr;

	for (int ContestantsCount = 0; ContestantsCount < GameInstance->Contestants; ContestantsCount++)
	{
		bool found = false;

		for (auto Start : StartingPoints)
		{
			APlayerStart* PlayerStart = Cast<APlayerStart>(Start);
			FString Name = PlayerStart->GetActorLabel();

			UE_LOG(LogTemp, Warning, TEXT("Considering actor %s for count %d"), *Name, ContestantsCount);

			FRegexMatcher Matcher(PlayerStartRegex, Name);
			int PlayerIndex = -1;

			if (Matcher.FindNext())
			{
				FString Match = Matcher.GetCaptureGroup(1);
				if (Match.IsNumeric())
					PlayerIndex = FCString::Atoi(*Match);
			}

			// Each player start correspond to a specific player
			if (PlayerIndex == -1 || PlayerIndex != ContestantsCount) continue;

			FVector PlayerLocation = Start->GetActorLocation();
			FRotator PlayerRotation = Start->GetActorRotation();

			//if (PlayerIndex == 0)
			//	GetWorld()->SpawnActor(CharacterClass, &PlayerLocation, &PlayerRotation);
			//else
			//	GetWorld()->SpawnActor(OpponentsClass, &PlayerLocation, &PlayerRotation);

			if (PlayerIndex == 0)
			{
				//			Character = Cast<APawn>(GetWorld()->SpawnActor(CharacterClass, &FVector::ZeroVector, &FRotator::ZeroRotator));
				FVector Location = Start->GetActorLocation();
				FRotator Rotation = Start->GetActorRotation();

				// WARNING check if it does deep copy inside spawn actor
				Character = Cast<APawn>(GetWorld()->SpawnActor(CharacterClass, &Location, &Rotation));
				Character->SetActorLocation(Start->GetActorLocation());
				Character->SetActorRotation(Start->GetActorRotation());
			}
			else
			{
				/*AActor* Opponent = GetWorld()->SpawnActor(OpponentsClass, &FVector::ZeroVector, &FRotator::ZeroRotator);*/
				FVector Location = Start->GetActorLocation();
				FRotator Rotation = Start->GetActorRotation();
				AActor* Opponent = GetWorld()->SpawnActor(OpponentsClass, &Location, &Rotation);
				Opponent->SetActorLocation(Start->GetActorLocation());
				Opponent->SetActorRotation(Start->GetActorRotation());
			}

			found = true;
			break;
		}

		if (!found)
		{
			UE_LOG(LogTemp, Error, TEXT("Player start with name PlayerStart_%d not found in map"), ContestantsCount);
			break;
		}
	}

	return Character;
}