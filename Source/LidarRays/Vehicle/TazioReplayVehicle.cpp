// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TazioReplayVehicle.h"
#include "TazioGameInstance.h"
#include "Csv/CsvParser.h"
#include <Engine/World.h>
#include "OdometryMessage.h"
#include <fstream>
#include "UDPSender.h"
#include "MessageSerializerComponent.h"
#include <string>

ATazioReplayVehicle::ATazioReplayVehicle()
{
}

void ATazioReplayVehicle::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Label: %s"), *GetActorLabel());

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
	
	// Initialize UDP sender
	StateSender = NewObject<AUDPSender>(this);

	FString SenderIP = GameInstance->OpponentStateSendIP;

	// Players have label Player_<index>
	FRegexPattern PlayerStartRegex("Player_([0-9]+)");
	FRegexMatcher Matcher(PlayerStartRegex, GetName());
	int PlayerIndex = -1;

	if (Matcher.FindNext())
	{
		FString Match = Matcher.GetCaptureGroup(1);
		if (Match.IsNumeric())
			PlayerIndex = FCString::Atoi(*Match);
	}

	if (PlayerIndex == -1)
		isValid = false;


	// Ports start from the one indicated in game instance upwards (-1 because player 0 has a different port)
	int SenderPort = GameInstance->OpponentStatePort + PlayerIndex - 1;

	StateSender->Start(GetActorLabel() + FString("_Sender"), SenderIP, SenderPort, true);

	MessageSerializerComponent = FindComponentByClass<UMessageSerializerComponent>();
	if (MessageSerializerComponent == nullptr)
	{
		isValid = false;
		UE_LOG(LogTemp, Error, TEXT("TazioReplayVehicle::BeginPlay --- Message serializer component not found"));
	}
}

void ATazioReplayVehicle::FillBuffer()
{
	while (!trajectory.isFull())
	{
		AddTrajectoryPoint();
	}
}

bool ATazioReplayVehicle::AddTrajectoryPoint()
{
	std::string line;

	if (replay_file.eof())
	{
		// Reset
		replay_file.seekg(0,replay_file.beg);
		isFirst = true;
		hasReset = true;
		return false;
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
		return false;

	TrajectoryPoint point;
	point.time = std::atof(TCHAR_TO_ANSI(Parser.GetRows()[0][0]));
	point.x = std::atof(TCHAR_TO_ANSI(Parser.GetRows()[0][1]));
	point.y = std::atof(TCHAR_TO_ANSI(Parser.GetRows()[0][2]));
	point.z = std::atof(TCHAR_TO_ANSI(Parser.GetRows()[0][3]));
	point.theta = std::atof(TCHAR_TO_ANSI(Parser.GetRows()[0][4]));
	point.v = std::atof(TCHAR_TO_ANSI(Parser.GetRows()[0][5]));

	trajectory.insert(point);
	return true;
}

void ATazioReplayVehicle::Tick(float Delta)
{
	Super::Tick(Delta);

	if (replay_file.fail() || !isValid)
		return;

	UE_LOG(LogTemp, Warning, TEXT("REPLAY TICKING: %f"), Delta);


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

	TrajectoryPoint oldestPoint = trajectory.peek(trajectory.capacity() - 1);
	TrajectoryPoint penultimatePoint = trajectory.peek(trajectory.capacity() - 2);

	// Pop only if you are past the penultimate point --> the oldest point is useless
	while (true)
	{
		// At each time current time must be within oldest and penultimate	
		oldestPoint = trajectory.peek(trajectory.capacity() - 1);
		penultimatePoint = trajectory.peek(trajectory.capacity() - 2);

		if (CurrentTime - InitTime > penultimatePoint.time)
		{
			trajectory.pop();
			if (AddTrajectoryPoint())
				continue;
			else
				return;
		}

		if (CurrentTime - InitTime >= oldestPoint.time && CurrentTime - InitTime < penultimatePoint.time)
			break;
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
	
	// Linear interpolation to find actual pose
	double new_x = oldestPoint.x + points_delta_x / points_delta_time * (CurrentTime - InitTime - oldestPoint.time);
	double new_y = oldestPoint.y + points_delta_y / points_delta_time * (CurrentTime - InitTime - oldestPoint.time);
	double new_z = oldestPoint.z + points_delta_z / points_delta_time * (CurrentTime - InitTime - oldestPoint.time);
	double new_theta = oldestPoint.theta + points_delta_theta / points_delta_time * (CurrentTime - InitTime - oldestPoint.time);
	double new_v = oldestPoint.v + points_delta_v / points_delta_time * (CurrentTime - InitTime - oldestPoint.time);

	// Set new actor pose
	SetActorLocation(FVector(new_x * 100, new_y * 100, new_z * 100));
	SetActorRotation(FRotator(0.f, new_theta*180.f/PI, 0.f));

	// Now serialize message and send it
	FOdometryMessage Pose;
	Pose.x = new_x;
	Pose.y = -new_y;
	Pose.z = new_z;

	// FIXME temp
	Pose.vx = 0.0;
	Pose.vy = 0.0;
	Pose.vz = 0.0;

	Pose.pitch = 0.0;
	Pose.roll = 0.0;
	Pose.yaw = -new_theta;

	TArray<uint8> serialized_pose = MessageSerializerComponent->SerializeOdometryMessage(Pose, true);
	StateSender->SendData(serialized_pose);
}