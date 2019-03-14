// Fill out your copyright notice in the Description page of Project Settings.

#include "TazioGameMode.h"
#include <Engine/World.h>
#include <Kismet/GameplayStatics.h>
#include "TazioGameInstance.h"
#include <GameFramework/PlayerStart.h>
#include <GameFramework/Pawn.h>
#include <Internationalization/Regex.h>

#include "Tests/TestAction.h"
#include "Tests/TestRules.h"
#include "Tests/TestStateConversion.h"

#include <sstream>
#include <string>
#include <memory>

void ATazioGameMode::BeginPlay()
{
	Super::BeginPlay();

	EnvironmentParameters env;
	env.AddEntry("safety_dist", 10.0);

	raceControl.UpdateEnvironmentParameters(GetWorld()->GetTimeSeconds(), env);
}

void ATazioGameMode::Tick(float DeltaTime)
{
	for (auto contestant : contestants)
	{
		State s;
		s.AddStateVariable("x");
		s.AddStateVariable("y");
		s.AddStateVariable("yaw");

		s("x") = contestant->GetActorLocation().X;
		s("y") = contestant->GetActorLocation().Y;
		s("yaw") = contestant->GetActorRotation().Yaw;

		Agent a;
		a.SetID(TCHAR_TO_UTF8(*contestant->GetName()));
		a.SetState(s);
		raceControl.Update(GetWorld()->GetTimeSeconds(), a);
	}

	raceControl.Run(GetWorld()->GetTimeSeconds());
	
	for (const auto& contestant : raceControl.Contestants())
	{
		auto results = contestant.Results();

		if (GetWorld()->GetTimeSeconds() - results.latest().time() > 0.01)
			continue;

		if (results.size() > 0)
		{
			const auto& rules = results.latest().value().second;
			for (const auto& rule : rules)
			{
				bool result = rule.getResult();

				if (result)
				{
					UE_LOG(LogTemp, Warning, TEXT("At time: %f - Agent %s - Rule %s is OK"),
						results.latest().time(),
						*FString(contestant.ID().c_str()),
						*FString(rule.getName().c_str()));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("At time: %f - Agent %s - Rule %s is VIOLATED"),
						results.latest().time(),
						*FString(contestant.ID().c_str()),
						*FString(rule.getName().c_str()));
				}
			}
		}
	}
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

	// Search for actors with name "PlayerStart_<index>"
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

			FActorSpawnParameters params;
			params.Name = FName(*(FString("Player_") + FString::FromInt(PlayerIndex)));

			// Register contestant to race control
			std::stringstream ss;
			std::string index_str;
			ss << PlayerIndex;
			ss >> index_str;

			raceControl.RegisterContestant(TCHAR_TO_UTF8(*params.Name.ToString()));

			if (PlayerIndex == 0)
			{
				//	Character = Cast<APawn>(GetWorld()->SpawnActor(CharacterClass, &FVector::ZeroVector, &FRotator::ZeroRotator));
				FVector Location = Start->GetActorLocation();
				FRotator Rotation = Start->GetActorRotation();

				// WARNING check if it does deep copy inside spawn actor. It does in 4.19
				Character = Cast<APawn>(GetWorld()->SpawnActor(CharacterClass, &Location, &Rotation, params));
				Character->SetActorLocation(Start->GetActorLocation());
				Character->SetActorRotation(Start->GetActorRotation());
				contestants.Add(Character);
			}
			else
			{
				FVector Location = Start->GetActorLocation();
				FRotator Rotation = Start->GetActorRotation();
				AActor* Opponent = GetWorld()->SpawnActor(OpponentsClass, &Location, &Rotation, params);
				Opponent->SetActorLocation(Start->GetActorLocation());
				Opponent->SetActorRotation(Start->GetActorRotation());
				contestants.Add(Opponent);
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

	for (auto& contestant : raceControl.Contestants())
	{
		contestant.actionManager().addListener(std::shared_ptr<TestAction>(new TestAction));
		contestant.ruleMonitor().setRules(std::shared_ptr<TestRules>(new TestRules));
	}

	raceControl.SetStateConversionFcn(&UnrealToUnicycle);
	raceControl.setTrajectoryCapacity(10);

	return Character;
}