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

#include <memory>

class Action;
class SocialRules;

class RaceControl
{
	std::set<std::string> contestants;
	std::set<std::string> state_vars;

	std::list<ActionManager> actionManagers;
	std::list<RuleMonitor> ruleMonitors;

public:
	RaceControl();

	// Add contestants to the race
	void RegisterContestant(std::string ID);

	// Add listeners
	void AddListener(std::shared_ptr<Action>);

	// Set rules
	void SetRules(std::shared_ptr<SocialRules>);

	// Function to be called to evaluate action and rules
	void Run(double time, std::list<TimedContainer<Agent> > agentsState);
};
