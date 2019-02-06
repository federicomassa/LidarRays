// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "UnicycleModel.h"
#include <math.h>

void UnicycleModel::initModel()
{
	params["m"] = 1000.f;

	// Friction
	params["mu"] = 1000;
	params["b"] = 10;

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

	int vSign;
	if (oldState.at("v") > 0)
		vSign = 1;
	else if (oldState.at("v") < 0)
		vSign = -1;
	else
		vSign = 0;

	UE_LOG(LogTemp, Warning, TEXT("Friction sign: %d"), vSign);
	UE_LOG(LogTemp, Warning, TEXT("Friction: %f"), vSign*params.at("mu"));

	currentState.at("x") = oldState.at("x") + oldState.at("v")*std::cos(oldState.at("theta"))*DeltaTime;
	currentState.at("y") = oldState.at("y") + oldState.at("v")*std::sin(oldState.at("theta"))*DeltaTime;
	currentState.at("v") = oldState.at("v") + (lastControlsApplied.at("Force") - vSign*params.at("mu") - oldState.at("v")*params.at("b")) / params.at("m")*DeltaTime;

	currentState.at("theta") = oldState.at("theta") + lastControlsApplied.at("Omega")*DeltaTime;
}

std::map<std::string, float> UnicycleModel::controlsToModel(const std::map<std::string, float>& inControl) const
{
	std::map<std::string, float> outControl;

	try
	{
		outControl["Force"] = inControl.at("Throttle")*params.at("m")*10;
		outControl["Omega"] = inControl.at("Steering");
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Error, TEXT("UnicycleModel::controlsToModel --- %s"), *FString(e.what()));
	}

	return outControl;
}

std::map<std::string, float> UnicycleModel::statesToModel(const std::map<std::string, float>& inState) const
{
	std::map<std::string, float> outState;

	try
	{
		outState["x"] = inState.at("x")/100.f;
		outState["y"] = inState.at("y")/100.f;

		// This method is only called at initialization
		outState["v"] = 0.f/100.f;

		outState["theta"] = inState.at("yaw")*3.14159/180.f;
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Error, TEXT("UnicycleModel::statesToModel --- %s"), *FString(e.what()));
	}

	return outState;

}

std::map<std::string, float> UnicycleModel::controlsToWorld(const std::map<std::string, float>& inControl) const
{
	std::map<std::string, float> outControl;

	try
	{
		outControl["Throttle"] = inControl.at("Force") / params.at("m");
		outControl["Steering"] = inControl.at("Omega");
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Error, TEXT("UnicycleModel::controlsToWorld --- %s"), *FString(e.what()));
	}

	return outControl;
}

std::map<std::string, float> UnicycleModel::statesToWorld(const std::map<std::string, float>& inState) const
{
	std::map<std::string, float> outState;

	try
	{
		outState["x"] = inState.at("x")*100.f;
		outState["y"] = inState.at("y")*100.f;
		outState["yaw"] = inState.at("theta")*180/3.14159;
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Error, TEXT("UnicycleModel::statesToWorld --- %s"), *FString(e.what()));
	}

	return outState;
}