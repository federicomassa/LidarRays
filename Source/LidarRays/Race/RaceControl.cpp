// Fill out your copyright notice in the Description page of Project Settings.

#include "RaceControl.h"
#include "LogFunctions.h"
#include "Action.h"
#include "SocialRules.h"
#include "RaceExceptions.h"


using namespace Race;

RaceControl::RaceControl()
{

}

void RaceControl::RegisterContestant(std::string ID)
{
	// Check if already registered
	for (auto itr = contestants.begin(); itr != contestants.end(); itr++)
	{
		if (itr->getID() == ID)
			return;
	}

	AgentTrajectory trajectory;
	trajectory.setID(ID);
	contestants.push_back(trajectory);

	actionManagers.push_back(ActionManager());
	actionManagers.back().setID(ID);
	RuleMonitor rMon(actionManagers.back());
}

void RaceControl::AddListener(std::shared_ptr<Action> a)
{
	for (auto itr = actionManagers.begin(); itr != actionManagers.end(); itr++)
	{
		itr->addListener(a);
	}
}

void RaceControl::SetRules(std::shared_ptr<SocialRules> rules)
{
	for (auto itr = ruleMonitors.begin(); itr != ruleMonitors.end(); itr++)
	{
		itr->setRules(rules);
	}
}

void RaceControl::SetStateConversionFcn(StateConversionFcn fcn)
{
	conversionFcn = fcn;
}

void RaceControl::Update(double time, Agent a)
{
	auto found_itr = contestants.end();
	for (auto itr = contestants.begin(); itr != contestants.end(); itr++)
	{
		if (itr->getID() == a.GetID())
		{
			found_itr = itr;
			break;
		}
	}

	if (found_itr == contestants.end())
	{
		throw UnregisteredContestantException(a.GetID().c_str());
	}

	if (conversionFcn == nullptr)
	{
		throw UninitializedException("StateConversionFcn");
	}

	State updated_state = conversionFcn(a.GetState());

	found_itr->addState(time, updated_state);

	// TODO manage maximum memory of trajectory
}

void RaceControl::Run(double time)
{
	for (auto itr = actionManagers.begin(); itr != actionManagers.end(); itr++)
	{
		std::string currentID = itr->getID();
		AgentTrajectory self;
		std::vector<AgentTrajectory> others;

		// Fill self containers
		for (auto agent = contestants.begin(); agent != contestants.end(); agent++)
		{
			// if self
			if (agent->getID() == currentID) 
			{
				self = *agent;
				break;
			}
			else
			{
				others.push_back(*agent);
			}
		}
		
		itr->run(time, self, others);
	}
}