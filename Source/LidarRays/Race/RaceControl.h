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
#include "Contestant.h"

#include <memory>
#include <vector>

class Action;
class SocialRules;
class State;
class StateConverter;

class RaceControl
{
	std::vector<Contestant> contestants;

public:
	RaceControl();
	virtual ~RaceControl() {};

	// Add contestants to the race
	void RegisterContestant(std::string ID);

	typedef State(*StateConversionFcn) (State);
	
	void SetStateConversionFcn(StateConversionFcn);
	void SetStateConverter(StateConverter*);


	// Update agent a. State is in the contestant state space, and it will be converted to 
	// race control state space variables. Returns the updated state in race control state space
	State& Update(double time, Agent a);

	// Function to be called to evaluate action and rules, after updating states
	void Run(double time);

	// Update environment parameters of RuleMonitor
	void UpdateEnvironmentParameters(double time, const EnvironmentParameters& params);

	// Set properties for rule monitor
	void SetProperties(const Properties& prop);

	const std::vector<Contestant>& Contestants() const { return contestants; }
	std::vector<Contestant>& Contestants() { return contestants; }
	void setTrajectoryCapacity(size_t);

	TimedContainer<EnvironmentParameters>& getEnvironmentParameters() { return contestants.back().ruleMonitor().getEnvironmentParameters(); }

	void UpdateContestantParameters(std::string, const AgentParameters&);

	Contestant* GetContestant(std::string ID);

	virtual void Evaluate(double) {};
private:
	// Function that convert from the contestants state space to the race control state space
	StateConversionFcn conversionFcn = nullptr;
	std::shared_ptr<StateConverter> stateConverter = nullptr;
};
