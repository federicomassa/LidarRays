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
#include "AgentTrajectory.h"

#include <memory>
#include <vector>

class Action;
class SocialRules;
class State;

class RaceControl
{
	std::vector<AgentTrajectory> contestants;

	std::vector<ActionManager> actionManagers;
	std::vector<RuleMonitor> ruleMonitors;

public:
	RaceControl();

	// Add contestants to the race
	void RegisterContestant(std::string ID);

	typedef State(*StateConversionFcn) (State);
	
	void SetStateConversionFcn(StateConversionFcn);

	// Update agent a. State is in the contestant state space, and it will be converted to 
	// race control state space variables
	void Update(double time, Agent a);

	// Function to be called to evaluate action and rules, after updating states
	void Run(double time);

	// Update environment parameters of RuleMonitor
	void UpdateEnvironmentParameters(double time, const EnvironmentParameters& params);

	// Set properties for rule monitor
	void SetProperties(const Properties& prop);

	std::vector<ActionManager>& GetActionManagers() {return actionManagers;}
	std::vector<RuleMonitor>& GetRuleMonitors() { return ruleMonitors; }


private:
	// Function that convert from the contestants state space to the race control state space
	StateConversionFcn conversionFcn = nullptr;
};
