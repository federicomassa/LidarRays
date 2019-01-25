// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "UDPSender.h" 
#include "UDPReceiver.h"
#include "TazioVehicle.generated.h"

class UPhysicalMaterial;
class UCameraComponent;
class USpringArmComponent;
class UTextRenderComponent;
class UInputComponent;
class UAudioComponent;
class UControlMessage;

UCLASS(config=Game)
class ATazioVehicle : public AWheeledVehicle
{
	GENERATED_BODY()

	// Reference to controller component
	UInputComponent* InputComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = UDP)
	AUDPSender* LidarSender = nullptr;

	UPROPERTY(EditAnywhere, Category = UDP)
	AUDPSender* IMUSender = nullptr;

	UPROPERTY(EditAnywhere, Category = UDP)
	AUDPSender* GPSSender = nullptr;

	UPROPERTY(EditAnywhere, Category = UDP)
	AUDPReceiver* UDPReceiver = nullptr;

	bool ManualDriving = true;

	TArray < FInputAxisBinding > AxisBindings;

public:
	ATazioVehicle();

	UFUNCTION(BlueprintCallable, Category = UDP)
	AUDPSender* GetLidarSender();

	UFUNCTION(BlueprintCallable, Category = UDP)
	AUDPSender* GetIMUSender();

	UFUNCTION(BlueprintCallable, Category = UDP)
	AUDPSender* GetGPSSender();

	UFUNCTION(BlueprintCallable, Category = UDP)
	AUDPReceiver* GetUDPReceiver();

	// Begin Pawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End Pawn interface

	// Begin Actor interface
	virtual void Tick(float Delta) override;

protected:
	virtual void BeginPlay() override;

	void ToggleManualDriving();

	UFUNCTION(BlueprintCallable, Category = Controller)
	bool IsManualDriveMode() const
	{
		return ManualDriving;
	}

public:
	// End Actor interface

	/** Handle pressing forwards */
	UFUNCTION(BlueprintCallable, Category = Controller)
	void SendControls(UControlMessage* ControlMessage);

	static const FName LookUpBinding;
	static const FName LookRightBinding;
};
