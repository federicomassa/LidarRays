// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "Buffer.h"
#include "Definitions.h"
#include "ControlMessage.h"
#include "PoseMessage.h"
#include <fstream>
#include <chrono>
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
class USensorManager;
class UMessageSerializerComponent;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class VehicleModel;

UCLASS(config=Game)
class ATazioVehicle : public AWheeledVehicle
{
	GENERATED_BODY()

	// Game instance to access permanent variables
	UTazioGameInstance* GameInstance = nullptr;

	UPROPERTY()
	USensorManager* SensManager = nullptr;

	UMessageSerializerComponent* MessageSerializerComponent;

	// Reference to controller component
	UInputComponent* InputComponent = nullptr;

	// Scene capture component (the one used in split screen)
	USceneCaptureComponent2D* ThirdPersonSceneComponent = nullptr;

	// Texture targets, one for each possible opponent, taken from content browser
	TArray<UTextureRenderTarget2D*> TextureTargets;

	// File where to dump trajectory in csv
	std::ofstream trajectory_dump;

	// Logger file if needed: FIXME
	std::ofstream debug_file;

	float InitRecordingTime = -1.f;
	bool isFirst = true;
	bool isRecordingTrajectory = false;

	int PlayerIndex = -1;

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

	UPROPERTY()
	AUDPReceiver* PoseReceiver = nullptr;

private:

	TArray < FInputAxisBinding > AxisBindings;

	TUniquePtr<FPoseMessage> currentPose;
	TUniquePtr<FPoseMessage> lastPose;

	// Store time (from clock) of previous message
	double currentTime;
	double lastTime;
	std::chrono::steady_clock::time_point initTime;

	USkeletalMeshComponent* Mesh = nullptr;

	UWheeledVehicleMovementComponent4W* PhysicsMovementComponent = nullptr;

	UPROPERTY()
	UPawnMovementComponent* ModelMovementComponent = nullptr;

public:
	ATazioVehicle();
	~ATazioVehicle();

	// Called on BeginPlay
	void Init();

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

	UFUNCTION(BlueprintCallable, Category = UDP)
	AUDPReceiver* GetPoseReceiver();

	UFUNCTION(BlueprintCallable, Category = Utility)
	int GetPlayerIndex();

	UFUNCTION(BlueprintCallable, Category = Utility)
	TArray<UTextureRenderTarget2D*> GetTextureTargets() const;


	USensorManager* GetSensorManager();

	// Begin Pawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End Pawn interface

	// Begin Actor interface
	virtual void Tick(float Delta) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void ToggleRecordTrajectory();

public:
	// End Actor interface

	/** Handle pressing forwards */
	UFUNCTION(BlueprintCallable, Category = Controller)
	void SendControls(const FControlMessage& ControlMessage);

	UFUNCTION(BlueprintCallable, Category = Controller)
	void SendPose(const FPoseMessage& PoseMessage);

	static const FName LookUpBinding;
	static const FName LookRightBinding;
};
