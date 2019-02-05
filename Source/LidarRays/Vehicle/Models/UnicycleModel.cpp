// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "UnicycleModel.h"
#include <math.h>

void UnicycleModel::initModel()
{
	params["m"] = 1000.f;

	stateVars.insert("x");
	stateVars.insert("y");
	stateVars.insert("v");
	stateVars.insert("theta");

	controlVars.insert("Force");
	controlVars.insert("Omega");
}

void UnicycleModel::closeModel()
{

}

void UnicycleModel::requestControl(std::map<std::string, float> controlRequest)
{
	lastControlsApplied = controlRequest;
}


void UnicycleModel::executeModel(float DeltaTime)
{
	std::map<std::string, float> oldState = currentState;

	currentState.at("x") = oldState.at("x") + oldState.at("v")*std::cos(oldState.at("theta"))*DeltaTime;
	currentState.at("y") = oldState.at("y") + oldState.at("v")*std::sin(oldState.at("theta"))*DeltaTime;
	currentState.at("v") = oldState.at("v") + lastControlsApplied.at("Force")/params.at("m")*DeltaTime;
	currentState.at("theta") = oldState.at("theta") + lastControlsApplied.at("Omega")*DeltaTime;
}

std::map<std::string, float> UnicycleModel::controlsToModel(const std::map<std::string, float>& inControl) const
{
	std::map<std::string, float> outControl;

	outControl["Force"] = inControl.at("Throttle")*params.at("m");
	outControl["Omega"] = inControl.at("Steering");

	return outControl;
}

std::map<std::string, float> UnicycleModel::statesToModel(const std::map<std::string, float>& inState) const
{
	std::map<std::string, float> outState;

	outState["x"] = inState.at("x");
	outState["y"] = inState.at("y");

	// This method is only called at initialization
	outState["v"] = 0.f;

	outState["theta"] = inState.at("yaw");
	
	return outState;

}

std::map<std::string, float> UnicycleModel::controlsToWorld(const std::map<std::string, float>& inControl) const
{
	std::map<std::string, float> outControl;

	outControl["Throttle"] = inControl.at("Force")/params.at("m");
	outControl["Steering"] = inControl.at("Omega");

	return outControl;
}

std::map<std::string, float> UnicycleModel::statesToWorld(const std::map<std::string, float>& inState) const
{
	std::map<std::string, float> outState;

	outState["x"] = inState.at("x");
	outState["y"] = inState.at("y");
	outState["yaw"] = inState.at("theta");

	return outState;
}