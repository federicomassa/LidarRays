// Fill out your copyright notice in the Description page of Project Settings.

#include "CarPlayerController.h"

#include <GameFramework/Pawn.h>
#include <Engine/World.h>
#include "Vehicle/TazioVehicle.h"
#include "WheeledVehicleMovementComponent.h"
#include "utility.h"

void ACarPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (GetPawn() == nullptr)
		UE_LOG(LogTemp, Error, TEXT("Player controller has no pawn"));

	GetPawn()->InputComponent->BindAction("SaveTrajectory", IE_Pressed, this, &ACarPlayerController::ToggleSaveTrajectory);
	GetPawn()->InputComponent->BindAction("ToggleILC", IE_Pressed, this, &ACarPlayerController::ToggleILC);
	GetPawn()->InputComponent->BindAction("ResetPosition", IE_Pressed, this, &ACarPlayerController::ResetPosition);

	initial_position = GetPawn()->GetActorLocation();
	initial_rotation = GetPawn()->GetActorRotation();

	output_trajectory.open("D:/output_trajectory.csv");
	output_trajectory << "time,x,y,theta,v" << std::endl;

	try
	{
		controller.configure("D:/ilc_config_test.txt");
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *FString(e.what()));
	}
}

void ACarPlayerController::Tick(float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("isSaving: %i"), isSaving);
	UE_LOG(LogTemp, Warning, TEXT("isILCActive: %i"), isILCActive);


	if (isSaving && (GetWorld()->GetTimeSeconds() - last_trajectory_time) > 1.f/trajectory_frequency)
	{
		FVector position = GetPawn()->GetActorLocation();
		FVector velocity = GetPawn()->GetVelocity();
		FRotator orientation = GetPawn()->GetActorRotation();

		output_trajectory << GetWorld()->GetTimeSeconds() - TrajectoryStartTime << ',' << position.X/100.f << ',' << position.Y/100.f << ',' << orientation.Yaw*PI/180.f << ',' << velocity.Size()/100.f << std::endl;
		last_trajectory_time = GetWorld()->GetTimeSeconds();
	}



	if (isILCActive)
	{
		if (GetWorld()->GetTimeSeconds() - ILCStartTime > controller.getReferenceDuration())
		{
			ToggleSaveTrajectory();
		}
		else 
		{
			if (GetWorld()->GetTimeSeconds() - ILCStartTime > controller.getReferenceDuration())
			{
				//end and restart experiment
				ToggleILC();
				ToggleILC();
				ResetPosition();
				return;
			}

			Utility::NumericMap current_state;

			FVector position = GetPawn()->GetActorLocation();
			FVector velocity = GetPawn()->GetVelocity();
			FRotator orientation = GetPawn()->GetActorRotation();

			Utility::NumericMap current_ref = controller.getReference(GetWorld()->GetTimeSeconds() - ILCStartTime);

			current_state["x"] = position.X / 100.f;
			current_state["y"] = position.Y / 100.f;
			current_state["theta"] = orientation.Yaw;
			current_state["v"] = velocity.Size()/100.f;

			// To normalize angle
			double angle_diff = current_ref.at("theta") - current_state.at("theta");
			if (std::abs(angle_diff) > 2*PI)
			{
				current_state.at("theta") += std::abs(angle_diff) / angle_diff * 2 * PI;
			}

			Utility::NumericMap control, purePursuitControl;

			try
			{
				control = controller.computePIDControl(current_state, GetWorld()->GetTimeSeconds() - ILCStartTime);
				purePursuitControl = controller.computePurePursuitControl(current_state, GetWorld()->GetTimeSeconds() - ILCStartTime);
			}
			catch (std::exception& e)
			{
				UE_LOG(LogTemp, Error, TEXT("Something wrong while computing controls: %s"), *FString(e.what()));
			}


			UE_LOG(LogTemp, Warning, TEXT("Steer: %f, Throttle: %f"), purePursuitControl.at("pure_pursuit"), control.at("throttle"));

			Cast<UWheeledVehicleMovementComponent>(GetPawn()->GetMovementComponent())->SetThrottleInput(control.at("throttle"));
			Cast<UWheeledVehicleMovementComponent>(GetPawn()->GetMovementComponent())->SetSteeringInput(purePursuitControl.at("pure_pursuit"));

		}
	}

}

void ACarPlayerController::ToggleSaveTrajectory()
{
	isSaving = !isSaving;

	if (!isSaving)
		last_trajectory_time = -1;

	if (isSaving)
		TrajectoryStartTime = GetWorld()->GetTimeSeconds();
}

void ACarPlayerController::ToggleILC()
{
	isILCActive = !isILCActive;

	if (isILCActive)
	{
		ILCStartTime = GetWorld()->GetTimeSeconds();
		controller.startExperiment();
	}
	else
	{
		controller.endExperiment();
		controller.dumpLatestControls();
	}

}

void ACarPlayerController::ResetPosition()
{
	//FVector DeltaPosition = initial_position - GetPawn()->GetActorLocation();
	//FRotator DeltaRotation = initial_rotation - GetPawn()->GetActorRotation();

	//GetPawn()->AddActorWorldOffset(DeltaPosition, false, nullptr, ETeleportType::TeleportPhysics);
	//GetPawn()->AddActorWorldRotation(DeltaRotation, false, nullptr, ETeleportType::TeleportPhysics);

	//// Try to stop
	//Cast<ATazioVehicle>(GetPawn())->GetRootComponent()->ComponentVelocity = FVector(0.f, 0.f, 0.f);
	//Cast<ATazioVehicle>(GetPawn())->GetMovementComponent()->UpdateComponentVelocity();

	GetPawn()->Destroy();
	GetWorld()->GetAuthGameMode()->SpawnPlayerFromSimulate(initial_position, initial_rotation);

	GetPawn()->InputComponent->BindAction("SaveTrajectory", IE_Pressed, this, &ACarPlayerController::ToggleSaveTrajectory);
	GetPawn()->InputComponent->BindAction("ToggleILC", IE_Pressed, this, &ACarPlayerController::ToggleILC);
	GetPawn()->InputComponent->BindAction("ResetPosition", IE_Pressed, this, &ACarPlayerController::ResetPosition);
}
