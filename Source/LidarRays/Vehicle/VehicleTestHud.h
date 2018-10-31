// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "VehicleTestHud.generated.h"

UCLASS(config = Game)
class AVehicleTestHud : public AHUD
{
	GENERATED_BODY()

public:
	AVehicleTestHud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface

};
