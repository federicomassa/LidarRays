// Fill out your copyright notice in the Description page of Project Settings.

#include "TazioGameMode.h"
#include <Engine/World.h>
#include <Kismet/GameplayStatics.h>
#include "TazioGameInstance.h"
#include <GameFramework/PlayerStart.h>
#include <GameFramework/Pawn.h>
#include <Internationalization/Regex.h>

#include "RRStateConverter.h"
#include "TimerAction.h"
#include "TriggerAttackAction.h"
#include "OvertakeAction.h"
#include "SeasonAlphaRules.h"

#include <sstream>
#include <string>
#include <memory>
#include "SeasonAlphaRaceControl.h"

ATazioGameMode::ATazioGameMode()
{
	raceControl = new SeasonAlphaRaceControl;
}

void ATazioGameMode::BeginPlay()
{
	Super::BeginPlay();

	UTazioGameInstance* GameInstance = Cast<UTazioGameInstance>(GetGameInstance());
	check(GameInstance);


	std::string track_map_path = TCHAR_TO_UTF8(*GameInstance->TrackMapFilePath);

	EnvironmentParameters env;
	if (raceControl->getEnvironmentParameters().size() != 0)
		env = raceControl->getEnvironmentParameters().latest().value();

	env.AddEntry("safety_dist", 10.0);

	RRStateConverter* stateConverter = new RRStateConverter(track_map_path, env);
	raceControl->UpdateEnvironmentParameters(GetWorld()->GetTimeSeconds(), env);

	raceControl->SetStateConverter(stateConverter);

	for (auto& contestant : raceControl->Contestants())
	{
		AgentParameters par;
		par.AddEntry("last_s", 0.0);
		par.AddEntry("lap", 0.0);
		par.AddEntry("race_mode", 0.0); // 0 = test mode, 1 = race active, 2 = race finished
		par.AddEntry("overtaking_status", 0.0); // 0 = neutral, 1 = attack, 2 = defend
		par.AddEntry("speed_limit", 200.0); // kph
		par.AddEntry("power_limit", 500.0); // kW
		par.AddEntry("race_position", 0.0);
		par.AddEntry("lap_remaining", 0.0);
		
		contestant.trajectory().setParameters(par);
	}
}

void ATazioGameMode::Tick(float DeltaTime)
{
	for (auto& actor : contestants)
	{
		Contestant* race_contestant = nullptr;

		for (auto& contestant : raceControl->Contestants())
		{
			if (FString(contestant.ID().c_str()) == actor->GetName())
			{
				race_contestant = &contestant;
				break;
			}
		}

		LogFunctions::Require(race_contestant != nullptr, "ATazioGameMode::Tick", "Contestant not found");

		State s;
		s.AddStateVariable("x");
		s.AddStateVariable("y");
		s.AddStateVariable("v");

		s("x") = actor->GetActorLocation().X;
		s("y") = actor->GetActorLocation().Y;
		s("v") = FMath::Sqrt(FMath::Pow(actor->GetVelocity().X, 2) + FMath::Pow(actor->GetVelocity().Y, 2));

		Agent a;
		a.SetID(TCHAR_TO_UTF8(*actor->GetName()));
		a.SetState(s);
		State& updated_state = raceControl->Update(GetWorld()->GetTimeSeconds(), a);

		double circuit_length = raceControl->getEnvironmentParameters().latest().value().at("total_circuit_length");

		// If a lap is over new s will be close to zero
		double ds_threshold = circuit_length*0.5; //m
		if (race_contestant->trajectory().parameter("last_s") > updated_state("s") + ds_threshold)
			race_contestant->trajectory().parameter("lap") += 1;

		race_contestant->trajectory().parameter("last_s") = updated_state("s");

		updated_state("s") += circuit_length * race_contestant->parameter("lap");
	}

	raceControl->Run(GetWorld()->GetTimeSeconds());
	raceControl->Evaluate(GetWorld()->GetTimeSeconds());
	
	for (const auto& contestant : raceControl->Contestants())
	{
		if (contestant.ID() != "Player_0")
			continue;

		double s = contestant.trajectory().getTrajectory().latest().value()("s");
		double y = contestant.trajectory().getTrajectory().latest().value()("y");
		double next_index = contestant.parameter("next_index");
		int current_zone_type = (int)contestant.parameter("current_zone_type");
		int current_zone_id = (int)contestant.parameter("current_zone_id");

		if (current_zone_type != -1 && current_zone_type != lastEgoZoneType)
		{
			UE_LOG(LogTemp, Warning, TEXT("Zone: %d"), current_zone_type);
			lastEgoZoneType = current_zone_type;
		}

		auto results = contestant.Results();

		for (const auto& action : results.ActionHistory)
		{
			if (action.triggerTime == GetWorld()->GetTimeSeconds())
			{
				UE_LOG(LogTemp, Warning, TEXT("Action %s was triggered"), *FString(action.name.c_str()));
			}
			else if (action.endTime == GetWorld()->GetTimeSeconds() && action.status == ENDED)
			{
				UE_LOG(LogTemp, Warning, TEXT("Action %s ended"), *FString(action.name.c_str()));
			}
			else if (action.endTime == GetWorld()->GetTimeSeconds() && action.status == ABORTED)
			{
				UE_LOG(LogTemp, Warning, TEXT("Action %s aborted"), *FString(action.name.c_str()));
			}
		}

		for (const auto& latest_results : results.RulesHistory)
		{
			// only consider latest results
			if (GetWorld()->GetTimeSeconds() - latest_results.time() > 0.0001)
				continue;

			const auto& rules = latest_results.value();

			for (const auto& rule : rules)
			{
				bool result = rule.getResult();

				if (result)
				{
					UE_LOG(LogTemp, Warning, TEXT("At time: %f - Agent %s - Rule %s is OK"),
						latest_results.time(),
						*FString(contestant.ID().c_str()),
						*FString(rule.getName().c_str()));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("At time: %f - Agent %s - Rule %s is VIOLATED"),
						latest_results.time(),
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

			raceControl->RegisterContestant(TCHAR_TO_UTF8(*params.Name.ToString()));

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

	for (auto& contestant : raceControl->Contestants())
	{
		//contestant.actionManager().addListener(std::shared_ptr<TimerAction>(new TimerAction));
		contestant.actionManager().addListener(std::shared_ptr<TriggerAttackAction>(new TriggerAttackAction));
		contestant.actionManager().addListener(std::shared_ptr<OvertakeAction>(new OvertakeAction));
		contestant.ruleMonitor().setRules(std::shared_ptr<SeasonAlphaRules>(new SeasonAlphaRules));
	}
	 
	// Save only last N trajectory points
	raceControl->setTrajectoryCapacity(10);

	return Character;
}


void ATazioGameMode::UpdateEnvironmentParameters(double time, EnvironmentParameters par)
{
	raceControl->UpdateEnvironmentParameters(time, par);
}