// Fill out your copyright notice in the Description page of Project Settings.

#include "RaceControl.h"

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

	contestants[ID];
}


