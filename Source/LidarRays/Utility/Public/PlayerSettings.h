#pragma once

#include "PlayerSettings.generated.h"

USTRUCT(Blueprintable)
struct FPlayerSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PlayerIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString VehicleIP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int VehiclePort;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GPSSendIP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int GPSSendPort;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LidarSendIP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int LidarSendPort;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLidarActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsGPSActive;

public:
	FPlayerSettings()
	{
		VehicleIP = "";
		VehiclePort = -1;
		LidarSendIP = "";
		LidarSendPort = -1;
		GPSSendIP = "";
		GPSSendPort = -1;
		ID = -1;
		PlayerIndex = -1;
		bIsLidarActive = false;
		bIsGPSActive = false;
	}
};