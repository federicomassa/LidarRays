// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <GameFramework/Pawn.h>
#include "Buffer.h"
#include "TrajectoryPoint.h"
#include <fstream>
#include "TazioReplayVehicle.generated.h"


UCLASS(config=Game)
class ATazioReplayVehicle : public APawn
{
	GENERATED_BODY()

	std::ifstream replay_file;
	Buffer<TrajectoryPoint> trajectory;
	bool isFirst = true;
	bool hasReset = false;
	float InitTime = -1.f;

private:
	bool AddTrajectoryPoint();
	void FillBuffer();
public:
	ATazioReplayVehicle();

	void BeginPlay() override;
	void Tick(float Delta) override;
};
