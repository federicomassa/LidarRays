// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SingleTrackModel.h"
#include <math.h>

void SingleTrackModel::initModel()
{
	// Distance from center to front and rear axles (meters)
	params["La"] = 1.245870f;
	params["Lr"] = 1.245870f;

	// Brake and traction front/rear repartition  
	params["Bb"] = 1.44f;
	params["Tb"] = 0.57f;

	// Vehicle mass
	params["m"] = 1270.f;

	params["B1"] = 14.2314990512334f;
	params["C1"] = 2.f;
	params["D1"] = 8432.f;
	params["E1"] = 0.748975728703042f;

	params["B2"] = 17.7035912999494f;
	params["C2"] = 2.f;
	params["D2"] = 9885.f;
	params["E2"] = 0.781939473634122f;

	// ?
	params["Jz"] = 1260.f;

	params["max_traction"] = 5000;
	params["max_brake"] = 12000;

	params["max_steering"] = 45 * 3.14159 / 180;

	// Longitudinal velocity, lateral velocity, yaw rate, center position, yaw
	stateVars.insert("u");
	stateVars.insert("v");
	stateVars.insert("r");
	stateVars.insert("xG");
	stateVars.insert("yG");
	stateVars.insert("psi");

	// Wheel angle, traction force, brake force (both positive)
	controlVars.insert("delta");
	controlVars.insert("FT");
	controlVars.insert("FB");

}


void SingleTrackModel::closeModel()
{

}

void SingleTrackModel::requestControl(std::map<std::string, float> controlRequest)
{
	lastControlsApplied = controlRequest;
}


void SingleTrackModel::executeModel(float DeltaTime)
{
	if (currentState.at("u") == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("SingleTrackModel::executeModel --- longitudinal velocity cannot be zero"));
		return;
	}
		
	float udot1 = currentState.at("v")*currentState.at("r");
	float udot2 = -getF1t() / params.at("m")*std::sin(lastControlsApplied.at("delta"));
	float udot3 = -getF1l() / params.at("m")*std::cos(lastControlsApplied.at("delta"));
	float udot4 = -getF2l() / params.at("m");

	UE_LOG(LogTemp, Warning, TEXT("UDOT: %f, %f, %f, %f"), udot1, udot2, udot3, udot4);
	float udot = udot1 + udot2 + udot3 + udot4;

	float vdot1 = -currentState.at("u")*currentState.at("r");
	float vdot2 = getF1t() / params.at("m")*std::cos(lastControlsApplied.at("delta"));
	float vdot3 = -getF1l() / params.at("m")*std::sin(lastControlsApplied.at("delta"));
	float vdot4 = getF2l() / params.at("m");
	UE_LOG(LogTemp, Warning, TEXT("VDOT: %f, %f, %f, %f"), vdot1, vdot2, vdot3, vdot4);
	float vdot = vdot1 + vdot2 + vdot3 + vdot4;

	float rdot1 = params.at("La")*getF1t() / params.at("Jz")*std::cos(lastControlsApplied.at("delta"));
	float rdot2 = -params.at("La")*getF1l() / params.at("Jz")*std::sin(lastControlsApplied.at("delta"));
	float rdot3 = -params.at("Lr")*getF2t() / params.at("Jz");
	UE_LOG(LogTemp, Warning, TEXT("RDOT: %f, %f, %f"), rdot1, rdot2, rdot3);

	float rdot = rdot1 + rdot2 + rdot3;

	float xGdot = currentState.at("u")*std::cos(currentState.at("psi")) - currentState.at("v")*std::sin(currentState.at("psi"));
	float yGdot = currentState.at("u")*std::sin(currentState.at("psi")) + currentState.at("v")*std::cos(currentState.at("psi"));
	float psidot = currentState.at("r");

	currentState.at("u") += udot * DeltaTime;
	currentState.at("v") += vdot * DeltaTime;
	currentState.at("r") += rdot * DeltaTime;
	currentState.at("xG") += xGdot * DeltaTime;
	currentState.at("yG") += yGdot * DeltaTime;
	currentState.at("psi") += psidot * DeltaTime;

	if (currentState.at("u") < 0.01)
		currentState.at("u") = 0.01;

	UE_LOG(LogTemp, Error, TEXT("NEW STATE: %f, %f, %f, %f, %f, %f"),
		currentState.at("u"),
		currentState.at("v"),
		currentState.at("r"),
		currentState.at("xG"),
		currentState.at("yG"),
		currentState.at("psi"));
}

std::map<std::string, float> SingleTrackModel::controlsToModel(const std::map<std::string, float>& inControl) const
{
	std::map<std::string, float> outControl;

	try
	{
		if (inControl.at("Throttle") > 0)
		{
			outControl["FT"] = inControl.at("Throttle")*params.at("max_traction");
			outControl["FB"] = 0.f;
		}
		else
		{
			outControl["FT"] = 0.f;
			outControl["FB"] = -inControl.at("Throttle")*params.at("max_brake");
		}

		outControl["delta"] = inControl.at("Steering")*params.at("max_steering");
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Error, TEXT("SingleTrackModel::controlsToModel --- %s"), *FString(e.what()));
	}

	return outControl;
}

std::map<std::string, float> SingleTrackModel::statesToModel(const std::map<std::string, float>& inState) const
{
	std::map<std::string, float> outState;

	try
	{
		// This method is only called at initialization. That's why velocities are zero

		// u cannot be zero
		outState["u"] = 0.01;
		outState["v"] = 0.f;
		outState["r"] = 0.f;
		outState["xG"] = inState.at("x")/100.f;
		outState["yG"] = inState.at("y") / 100.f;

		outState["psi"] = inState.at("yaw")*3.14159/180.f;
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Error, TEXT("SingleTrackModel::statesToModel --- %s"), *FString(e.what()));
	}

	return outState;

}

std::map<std::string, float> SingleTrackModel::controlsToWorld(const std::map<std::string, float>& inControl) const
{
	std::map<std::string, float> outControl;

	try
	{
		if (inControl.at("FT") > 0)
			outControl["Throttle"] = inControl.at("FT") / (params.at("max_traction"));
		else if (inControl.at("FB") > 0)
			outControl["Throttle"] = -inControl.at("FB") / (params.at("max_brake"));
		else
			outControl["Throttle"] = 0.f;

		outControl["Steering"] = inControl.at("delta")/params.at("max_steering");
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Error, TEXT("SingleTrackModel::controlsToWorld --- %s"), *FString(e.what()));
	}

	return outControl;
}

std::map<std::string, float> SingleTrackModel::statesToWorld(const std::map<std::string, float>& inState) const
{
	std::map<std::string, float> outState;

	try
	{
		outState["x"] = inState.at("xG")*100.f;
		outState["y"] = inState.at("yG")*100.f;
		outState["yaw"] = inState.at("psi")*180/3.14159;
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Error, TEXT("SingleTrackModel::statesToWorld --- %s"), *FString(e.what()));
	}

	return outState;
}

float SingleTrackModel::getAlpha1()
{
	float argument = 100000000;

	if (currentState.at("u") > 0.01)
		argument = (currentState.at("v") + currentState.at("r")*params.at("La")) / currentState.at("u");

	UE_LOG(LogTemp, Warning, TEXT("Alpha1 argument: %f"), argument);

	return (lastControlsApplied.at("delta") - std::atan(argument));
}

float SingleTrackModel::getAlpha2()
{
	float argument = 100000000;

	float result;

	if (currentState.at("u") > 0.01)
		argument = (currentState.at("v") - currentState.at("r")*params.at("Lr")) / currentState.at("u");

	result = -std::atan(argument);

	return result;
}

float SingleTrackModel::getF1t()
{
	try
	{
		float alpha1 = getAlpha1();
		float argument = params.at("B1")*alpha1 - params.at("E1")*(params.at("B1")*alpha1 - std::atan(params.at("B1")*alpha1));

		float result = params.at("D1")*std::sin(params.at("C1")*std::atan(argument));

		UE_LOG(LogTemp, Warning, TEXT("F1T: %f, %f, %f"), alpha1, argument, result);
		return result;
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Error, TEXT("SingleTrackModel::getF1t --- %s"), *FString(e.what()));
		return -1;
	}
	
}

float SingleTrackModel::getF2t()
{
	try
	{
		float alpha2 = getAlpha2();
		float argument = params.at("B2")*alpha2 - params.at("E2")*(params.at("B2")*alpha2 - std::atan(params.at("B2")*alpha2));
		return (params.at("D2")*std::sin(params.at("C2")*std::atan(argument)));
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Error, TEXT("SingleTrackModel::getF2t --- %s"), *FString(e.what()));
		return -1;
	}

}

float SingleTrackModel::getF1l()
{
	try
	{
		if (!isBraking())
			return (-(params.at("Tb") / (1 + params.at("Tb"))*lastControlsApplied.at("FT")));
		else
				return params.at("Bb") / (1 + params.at("Bb"))*lastControlsApplied.at("FB");
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Error, TEXT("SingleTrackModel::getF1l --- %s"), *FString(e.what()));
		return -1;
	}

}

float SingleTrackModel::getF2l()
{
	try
	{
		if (!isBraking())
			return (-(1.f / (1 + params.at("Tb"))*lastControlsApplied.at("FT")));
		else
				return 1.f / (1 + params.at("Bb"))*lastControlsApplied.at("FB");
	}
	catch (std::exception& e)
	{
		UE_LOG(LogTemp, Error, TEXT("SingleTrackModel::getF2l --- %s"), *FString(e.what()));
		return -1;
	}

}

bool SingleTrackModel::isBraking()
{
	if (lastControlsApplied.at("FT") > 0 && lastControlsApplied.at("FB") == 0)
	{
		return false;
	}
	else if (lastControlsApplied.at("FT") == 0 && lastControlsApplied.at("FB") >= 0)
	{
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SingleTrackModel::isBraking --- there's something wrong with controls!"));
		return true;
	}
}


