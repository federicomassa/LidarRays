// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TazioReplayVehicle.h"
#include "TazioGameInstance.h"
#include "Csv/CsvParser.h"
#include <Engine/World.h>
#include <fstream>
#include <string>

ATazioReplayVehicle::ATazioReplayVehicle()
{
}

void ATazioReplayVehicle::BeginPlay()
{
	Super::BeginPlay();

	UTazioGameInstance* GameInstance = Cast<UTazioGameInstance>(GetGameInstance());
	check(GameInstance);

	replay_file.open(TCHAR_TO_ANSI(*GameInstance->ReplayTrajectoryPath));

	if (!replay_file.good())
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot open file: %s"), *GameInstance->ReplayTrajectoryPath);
		return;
	}

	// Always keep 2 points ahead. Current point will be always within these two to linear interpolate
	trajectory.setSize(2);

	FillBuffer();
	
}

void ATazioReplayVehicle::FillBuffer()
{
	while (!trajectory.isFull())
	{
		AddTrajectoryPoint();
	}
}

void ATazioReplayVehicle::AddTrajectoryPoint()
{
	std::string line;

	if (replay_file.eof())
	{
		// Reset
		replay_file.seekg(0,replay_file.beg);
		isFirst = true;
		hasReset = true;
		return;
	}

	std::getline(replay_file, line);

	FString String(line.c_str());
	FCsvParser Parser(String);

	int columns = Parser.GetRows()[0].Num();

	// CSV must have 6 columns: time, x, y, z, theta, v
	check(columns == 6);

	// Check if row is numeric or header by evaluating the first element
	FString firstElement = Parser.GetRows()[0][0];
	
	if (!firstElement.IsNumeric())
		return;

	TrajectoryPoint point;
	point.time = FCString::Atof(Parser.GetRows()[0][0]);
	point.x = FCString::Atof(Parser.GetRows()[0][1]);
	point.y = FCString::Atof(Parser.GetRows()[0][2]);
	point.z = FCString::Atof(Parser.GetRows()[0][3]);
	point.theta = FCString::Atof(Parser.GetRows()[0][4]);
	point.v = FCString::Atof(Parser.GetRows()[0][5]);

	UE_LOG(LogTemp, Warning, TEXT("Parse: %s, %s, %s, %s, %s, %s"), 
		Parser.GetRows()[0][0], 
		Parser.GetRows()[0][1],
		Parser.GetRows()[0][2],
		Parser.GetRows()[0][3],
		Parser.GetRows()[0][4],
		Parser.GetRows()[0][5]);

	UE_LOG(LogTemp, Warning, TEXT("Parse: %f, %f, %f, %f, %f, %f"),
		FCString::Atof(Parser.GetRows()[0][0]),
		FCString::Atof(Parser.GetRows()[0][1]),
		FCString::Atof(Parser.GetRows()[0][2]),
		FCString::Atof(Parser.GetRows()[0][3]),
		FCString::Atof(Parser.GetRows()[0][4]),
		FCString::Atof(Parser.GetRows()[0][5]));




	trajectory.insert(point);
}

void ATazioReplayVehicle::Tick(float Delta)
{
	Super::Tick(Delta);

	UE_LOG(LogTemp, Warning, TEXT("TICKING"));

	float CurrentTime = GetWorld()->GetTimeSeconds();

	if (isFirst)
	{
		InitTime = CurrentTime;
		isFirst = false;
	}

	if (hasReset)
	{
		trajectory.empty();
		FillBuffer();
		hasReset = false;
	}

	// Peek oldest point
	TrajectoryPoint oldestPoint = trajectory.peek(trajectory.capacity() - 1);
	TrajectoryPoint penultimatePoint = trajectory.peek(trajectory.capacity() - 2);

	// Pop only if you are past the penultimate point --> the oldest point is useless
	if (CurrentTime - InitTime > penultimatePoint.time)
	{
		trajectory.pop();
		AddTrajectoryPoint();
	}

	double points_delta_time = penultimatePoint.time - oldestPoint.time;
	double points_delta_x = penultimatePoint.x - oldestPoint.x;
	double points_delta_y = penultimatePoint.y - oldestPoint.y;
	double points_delta_z = penultimatePoint.z - oldestPoint.z;
	double points_delta_theta = penultimatePoint.theta - oldestPoint.theta;

	// unwrap
	while (points_delta_theta > PI)
		points_delta_theta -= 2*PI;
	while (points_delta_theta < -PI)
		points_delta_theta += 2*PI;

	double points_delta_v = penultimatePoint.v - oldestPoint.v;
	
	double new_x = penultimatePoint.x + points_delta_x / points_delta_time * (CurrentTime - InitTime - penultimatePoint.time);
	double new_y = penultimatePoint.y + points_delta_y / points_delta_time * (CurrentTime - InitTime - penultimatePoint.time);
	double new_z = penultimatePoint.z + points_delta_z / points_delta_time * (CurrentTime - InitTime - penultimatePoint.time);
	double new_theta = penultimatePoint.theta + points_delta_theta / points_delta_time * (CurrentTime - InitTime - penultimatePoint.time);
	double new_v = penultimatePoint.v + points_delta_v / points_delta_time * (CurrentTime - InitTime - penultimatePoint.time);

	UE_LOG(LogTemp, Warning, TEXT("Deltas: %f, %f, %f, %f, %f, %f"), points_delta_time, points_delta_x, points_delta_y, points_delta_z, points_delta_theta, points_delta_v);
	UE_LOG(LogTemp, Warning, TEXT("New: %f, %f, %f, %f, %f, %f"), CurrentTime, new_x, new_y, new_z, new_theta, new_v);


	SetActorLocation(FVector(new_x * 100, new_y * 100, new_z * 100));
	SetActorRotation(FRotator(0.f, new_theta, 0.f));
}