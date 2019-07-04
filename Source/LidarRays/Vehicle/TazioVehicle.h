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
#include <Containers/Set.h>
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIDChangedDelegate, const int&, NewID);

UCLASS(config=Game)
class ATazioVehicle : public AWheeledVehicle
{
	GENERATED_BODY()

	// =========== COMPONENTS =========== // 
	UPROPERTY(Category = TazioVehicle, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UGPSComponent* GPSComponent;

	UPROPERTY(Category = TazioVehicle, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class ULidarComponent* LidarComponent;

	UPROPERTY(Category = TazioVehicle, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMessageSerializerComponent* MessageSerializerComponent;

	// Game instance to access permanent variables
	UTazioGameInstance* GameInstance = nullptr;

	UPROPERTY()
	USensorManager* SensManager = nullptr;

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

	// ID is a unique identifier
	int _ID = -1;

	// Player index is an increasing number from 1 to N (N contestants)
	int _PlayerIndex = -1;

	// If player settings are available for this player
	bool _PlayerSettingsReady = false;

	// If ID event has been received
	bool _IDReceived = false;

	//boost::asio::io_service io_service;
	//simulink::udp_receiver<FPoseMessage>* PoseReceiver = nullptr;

	// Event called when ID is reassigned
	UPROPERTY(BlueprintAssignable, Category = TazioVehicle)
	FIDChangedDelegate OnIDChanged;

	class FGPSSensor* GPSSensor = nullptr;
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

	bool WaitForPlayerSettings();


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

	UFUNCTION(BlueprintCallable, Category = TazioVehicle)
	USensorManager* GetSensorManager();

	UFUNCTION(BlueprintCallable, Category = TazioVehicle)
	void SetGPSActive(bool bNewActive);

	UFUNCTION(BlueprintCallable, Category = TazioVehicle)
	void SetLidarActive(bool bNewActive);

	UFUNCTION(BlueprintCallable)
	void IDReceived();

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

	UFUNCTION(BlueprintCallable, Category = TazioVehicle)
	int GetID() const { return _ID; }

	UFUNCTION(BlueprintCallable, Category = TazioVehicle)
	void SetID(const int& ID);

	UFUNCTION(BlueprintCallable, Category = TazioVehicle)
	void SetPlayerIndex(const int& PlayerIndex);

	FPoseMessage* GetLatestState() const { return currentPose.Get(); }

	static const FName LookUpBinding;
	static const FName LookRightBinding;
};
