#pragma once

#include "PlayerSettings.generated.h"

USTRUCT(Blueprintable)
struct FPlayerSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Address;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Port;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ID;

public:
	FPlayerSettings()
	{
		Address = "";
		Port = -1;
		ID = -1;
	}
};