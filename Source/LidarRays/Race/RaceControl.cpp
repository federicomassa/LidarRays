// Fill out your copyright notice in the Description page of Project Settings.

#include "RaceControl.h"
#include "LogFunctions.h"
#include "Action.h"
#include "SocialRules.h"
#include <AgentTrajectory.h>


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

	if (inserted)
	{
		actionManagers.push_back(ActionManager());
		actionManagers.back().setID(ID);
		RuleMonitor rMon(actionManagers.back());
	}
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

void RaceControl::Run(double time, std::vector<AgentTrajectory> agentsState)
{
	for (auto itr = actionManagers.begin(); itr != actionManagers.end(); itr++)
	{
		std::string currentID = itr->getID();
		AgentTrajectory self;
		std::vector<AgentTrajectory> others;

		// Fill self containers
		for (auto agent = agentsState.begin(); agent != agentsState.end(); agent++)
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