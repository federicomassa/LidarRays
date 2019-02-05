// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "UDPSender.h" 
#include "VehicleModel.h"
#include "UDPReceiver.h"
#include "CriticalSection.h"
#include "ControlMessage.h"
#include "TazioVehicle.generated.h"

class UPhysicalMaterial;
class UCameraComponent;
class USpringArmComponent;
class UTextRenderComponent;
class UInputComponent;
class UAudioComponent;

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
	AUDPSender* GPSTruthSender = nullptr;

	UPROPERTY(EditAnywhere, Category = UDP)
	AUDPReceiver* ControlReceiver = nullptr;

	bool ManualDriving = true;


	// Vehicle model
	UPROPERTY(EditAnywhere, Category = Model)
	EVehicleModelEnum VehicleModelType = EVehicleModelEnum::VM_PhysX;

	VehicleModel* DynamicModel = nullptr;

	bool bPhysXSimulation = true;

	TArray < FInputAxisBinding > AxisBindings;

	float lastDeltaTime = 0.f;

public:
	ATazioVehicle();
	~ATazioVehicle();


	UFUNCTION(BlueprintCallable, Category = UDP)
	AUDPSender* GetLidarSender();

	UFUNCTION(BlueprintCallable, Category = UDP)
	AUDPSender* GetIMUSender();

	UFUNCTION(BlueprintCallable, Category = UDP)
	AUDPSender* GetGPSSender();

	UFUNCTION(BlueprintCallable, Category = UDP)
	AUDPSender* GetGPSTruthSender();

	UFUNCTION(BlueprintCallable, Category = UDP)
	AUDPReceiver* GetControlReceiver();	

	// Begin Pawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End Pawn interface

	// Begin Actor interface
	virtual void Tick(float Delta) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


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
	void SendControls(const FControlMessage& ControlMessage);

	static const FName LookUpBinding;
	static const FName LookRightBinding;
};
