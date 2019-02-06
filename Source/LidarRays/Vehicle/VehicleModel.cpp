// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleModel.h"
#include "VehicleModels.h"

VehicleModel* VehicleModel::generateVehicleModel(EVehicleModelEnum modelType)
{
	if (modelType == EVehicleModelEnum::VM_PhysX)
		return nullptr;
	else if (modelType == EVehicleModelEnum::VM_Unicycle)
	{
		return new UnicycleModel();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("VehicleModel::generateVehicleModel --- unrecognized vehicle model type! Have you updated this function with the new model?"));
	}

	return nullptr;
}

void VehicleModel::setState(std::map<std::string, float> state)
{
	currentState = statesToModel(state);

	if (!isStateCompatible(currentState))
		UE_LOG(LogTemp, Error, TEXT("VehicleModel::setState --- currentState is incompatible with model. Something is wrong in the model or in the input"));
}

void VehicleModel::run(std::map<std::string, float> worldControlRequest, float DeltaTime)
{
	// Convert controls to model variables
	std::map<std::string, float> controlRequest = controlsToModel(worldControlRequest);

	if (!isControlCompatible(controlRequest))
		UE_LOG(LogTemp, Error, TEXT("VehicleModel::run --- could not convert control request to model variables. Something is wrong either in the input or in the model"));

	requestControl(controlRequest);

	if (!isControlCompatible(lastControlsApplied))
		UE_LOG(LogTemp, Error, TEXT("VehicleModel::run --- could not apply controls. Did you set lastControlsApplied inside requestControl method?"));

	executeModel(DeltaTime);

	if (!isStateCompatible(currentState))
		UE_LOG(LogTemp, Error, TEXT("VehicleModel::run --- currentState is not compatible with model. Did you set the initial state with setState? Did you update it in the update() method?"));
}


bool VehicleModel::isStateCompatible(std::map<std::string, float> map)
{	
	// Check size
	if (map.size() != stateVars.size())
		return false;

	for (auto itr = map.begin(); itr != map.end(); itr++)
	{
		if (stateVars.find(itr->first) == stateVars.end())
			return false;
	}

	return true;
}

bool VehicleModel::isControlCompatible(std::map<std::string, float> map)
{
	// Check size
	if (map.size() != controlVars.size())
		return false;

	for (auto itr = map.begin(); itr != map.end(); itr++)
	{
		if (controlVars.find(itr->first) == controlVars.end())
			return false;
	}

	return true;
}

std::map<std::string, float> VehicleModel::getWorldState() const
{
	return statesToWorld(currentState);
}
