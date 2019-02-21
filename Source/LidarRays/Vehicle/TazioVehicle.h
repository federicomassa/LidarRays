// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "VehicleModel.h"
#include "CriticalSection.h"
#include "ControlMessage.h"
#include "Buffer.h"
#include "TazioVehicle.generated.h"

class UPhysicalMaterial;
class UCameraComponent;
class USpringArmComponent;
class UTextRenderComponent;
class UInputComponent;
class UAudioComponent;
class USkeletalMeshComponent;
class UWheeledVehicleMovementComponent4W;
class UPawnMovementComponent;
class UTazioGameInstance;
class AUDPSender;
class AUDPReceiver;

UCLASS(config=Game)
class ATazioVehicle : public AWheeledVehicle
{
	GENERATED_BODY()

	// Game instance to access permanent variables
	UTazioGameInstance* GameInstance = nullptr;

	// Reference to controller component
	UInputComponent* InputComponent = nullptr;

	AUDPSender* LidarSender = nullptr;

	AUDPSender* IMUSender = nullptr;

	AUDPSender* GPSSender = nullptr;

	AUDPSender* GPSTruthSender = nullptr;

	AUDPReceiver* ControlReceiver = nullptr;

	bool ManualDriving = true;

	//// Vehicle model
	//UPROPERTY(EditAnywhere, Category = Model)
	EVehicleModelEnum VehicleModelType = EVehicleModelEnum::VM_PhysX;

	VehicleModel* DynamicModel = nullptr;

	TArray < FInputAxisBinding > AxisBindings;

	float lastDeltaTime = 0.f;

	float lastThrottle = 0.f;
	float lastSteer = 0.f;

	USkeletalMeshComponent* Mesh = nullptr;

	UWheeledVehicleMovementComponent4W* PhysicsMovementComponent = nullptr;
	UPawnMovementComponent* ModelMovementComponent = nullptr;

	// When in kinematic model mode, controls are consumed inside Tick, possibly with a different frequency wrt the controller frequency
	Buffer<FControlMessage> controls;

public:
	ATazioVehicle();
	~ATazioVehicle();

	// Called on BeginPlay
	void Init();

	UPROPERTY(BlueprintReadOnly)
	bool bPhysXSimulation = true;

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

	void SetThrottle(float value);
	void SetSteer(float value);

	static const FName LookUpBinding;
	static const FName LookRightBinding;
};
