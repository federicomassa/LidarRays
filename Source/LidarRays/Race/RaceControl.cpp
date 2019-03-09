// Fill out your copyright notice in the Description page of Project Settings.

#include "RaceControl.h"
#include "LogFunctions.h"

using namespace Race;

RaceControl::RaceControl()
{

}

void RaceControl::RegisterContestant(std::string ID)
{
	if (state_vars.size() == 0)
	{
		throw UninitializedException("state_vars");
	}

	bool inserted = contestants.insert(ID).second;

	// If contestant was not already registered
	if (inserted)
	{
		ActionManager aMan;
		// TODO CONTINUE FROM HERE
	}
	else
	{
		LogFunctions::Error("RaceControl::RegisterContestant", std::string("User ") + ID + " was already registered");
	}
}

void RaceControl::Build()
{

}

