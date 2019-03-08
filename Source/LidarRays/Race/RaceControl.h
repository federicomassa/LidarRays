// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>
#include <string>
#include <vector>
#include "Timeseries.h"
#include "RaceExceptions.h"

class RaceControl
{
	std::map<std::string, Timeseries<double> > contestants;
	std::vector<std::string> state_vars;
public:
	RaceControl();
	void RegisterContestant(std::string ID);

};
