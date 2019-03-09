// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <set>
#include <string>
#include <vector>
#include <list>
#include "TimedContainer.h"
#include "ActionManager.h"
#include "Agent.h"
#include "RaceExceptions.h"
#include "RuleMonitor.h"

class RaceControl
{
	std::set<std::string> contestants;
	std::set<std::string> state_vars;

	std::list<ActionManager> agentManagers;
	std::list<RuleMonitor> ruleMonitors;

public:
	RaceControl();

	// Add contestants to the race
	void RegisterContestant(std::string ID);

	// After every initialization is carried out, set rules and managers
	void Build();
};
