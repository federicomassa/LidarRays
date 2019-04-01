// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "Buffer.h"
#include "Definitions.h"
#include "ControlMessage.h"
#include "Components/InputComponent.h"
#include <fstream>
#include "TazioVehicle.generated.h"

class UPhysicalMaterial;
class UCameraComponent;
class USpringArmComponent;
class UTextRenderComponent;
class UAudioComponent;
class USkeletalMeshComponent;
class UWheeledVehicleMovementComponent4W;
class UPawnMovementComponent;
class UTazioGameInstance;
class AUDPSender;
class AUDPReceiver;
class USensorManager;
class UMessageSerializerComponent;
class VehicleModel;

UCLASS(config=Game)
class ATazioVehicle : public AWheeledVehicle
{
	GENERATED_BODY()

	// Game instance to access permanent variables
	UTazioGameInstance* GameInstance = nullptr;

	UPROPERTY()
	USensorManager* SensManager = nullptr;

	UMessageSerializerComponent* MessageSerializerComponent = nullptr;

	// Reference to controller component
	UInputComponent* InputComponent = nullptr;

	// File where to dump trajectory in csv
	std::ofstream trajectory_dump;
	float InitRecordingTime = -1.f;
	bool isFirst = true;
	bool isRecordingTrajectory = false;


	// !!! NB: UPROPERTY() needed to avoid garbage collection !!!
public:
	UPROPERTY()
	AUDPSender* LidarSender = nullptr;

	UPROPERTY()
	AUDPSender* IMUSender = nullptr;

	UPROPERTY()
	AUDPSender* GPSSender = nullptr;

	UPROPERTY()
	AUDPSender* GPSTruthSender = nullptr;

	UPROPERTY()
	AUDPReceiver* ControlReceiver = nullptr;

private:

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

	UPROPERTY()
	UPawnMovementComponent* ModelMovementComponent = nullptr;

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

	USensorManager* GetSensorManager();

	const UMessageSerializerComponent* GetMessageSerializerComponent() const { return MessageSerializerComponent; }

	// Begin Pawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End Pawn interface

	// Begin Actor interface
	virtual void Tick(float Delta) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	void ToggleManualDriving();
	void ToggleRecordTrajectory();

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
