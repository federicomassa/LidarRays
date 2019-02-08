// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <map>
#include <set>
#include <string>

UENUM(BlueprintType)
enum class EVehicleModelEnum : uint8
{
	VM_PhysX UMETA(DisplayName = "PhysX"),
	VM_Unicycle UMETA(DisplayName = "Unicycle"),
	VM_SingleTrack UMETA(DisplayName = "SingleTrack")
};

class VehicleModel
{
protected:
	// in model variables
	std::map<std::string, double> currentState;
	std::map<std::string, double> lastControlsApplied;

	// Initialize these in initModel method
	std::map<std::string, double> params;
	std::set<std::string> stateVars;
	std::set<std::string> controlVars;

	// ====== Update model state via lastControlsApplied. This is the function where the actual dynamic model is written ===== //
	virtual void executeModel(double DeltaTime) = 0;

	// =============
	// World variables, e.g. x,y,theta could be in a different representation with respect to the model, e.g. x_front, x_rear, ..., roll, pitch, ..., velocity
	// =============

	// Convert input controls to model controls if necessary
	virtual std::map<std::string, double> controlsToModel(const std::map<std::string, double>& inControl) const { return inControl; };

	// Convert input state vars to model state vars if necessary
	virtual std::map<std::string, double> statesToModel(const std::map<std::string, double>& inState) const { return inState; };

	// Convert input controls to world controls if necessary
	virtual std::map<std::string, double> controlsToWorld(const std::map<std::string, double>& inControl) const { return inControl; };

	// Convert input state vars to world state vars if necessary
	virtual std::map<std::string, double> statesToWorld(const std::map<std::string, double>& inState) const { return inState; };

	// Here you can simulate a low level control (relation between control request and control applied) 
	// Send controls to model. NB Control in model variables. Sets lastControlsApplied in modelVariables
	virtual void requestControl(std::map<std::string, double> controlRequest) = 0;

public:
	// Initialize model: parameters, state variables, control variables
	virtual void initModel() {};

	// Close model
	virtual void closeModel() {};
	
	// Check states and controls compatibility and apply them to the model. Update currentState and lastControlsApplied
	void run(std::map<std::string, double> controlRequest, double DeltaTime);

public:
	// VehicleModel factory
	static VehicleModel* generateVehicleModel(EVehicleModelEnum modelType);

	// Input in world variables
	void setState(std::map<std::string, double> state);

	// Check if argument is compatible with stateVars
	bool isStateCompatible(std::map<std::string, double> map);

	// Check if argument is compatible with controlVars
	bool isControlCompatible(std::map<std::string, double> map);

	std::map<std::string, double> getWorldState() const;

	virtual ~VehicleModel() {}
};
	