// Fill out your copyright notice in the Description page of Project Settings.

#include "RaceControl.h"
#include "LogFunctions.h"
#include "Action.h"
#include "SocialRules.h"

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
		ActionManager aMan;
		aMan.setID(ID);
		const ActionManager& aMan_inserted = *(actionManagers.insert(aMan).first);

		RuleMonitor rMon(aMan_inserted);

	}
}

void RaceControl::AddListener(std::shared_ptr<Action> a)
{
	std::list<ActionManager>::iterator itr;
	for (itr = actionManagers.begin(); itr != actionManagers.end(); itr++)
	{
		itr->addListener(a);
	}
}

void RaceControl::SetRules(std::shared_ptr<SocialRules> rules)
{
	std::list<RuleMonitor>::iterator itr;
	for (itr = ruleMonitors.begin(); itr != ruleMonitors.end(); itr++)
	{
		itr->setRules(rules);
	}
}

void RaceControl::Run(double time, std::list<TimedContainer<Agent> > agentsState)
{
	for (auto itr = actionManagers.begin(); itr != actionManagers.end(); itr++)
	{
		std::string currentID = itr->getID();
		TimedContainer<Agent> self;
		
		// Fill self containers
		for (auto agent = agentsState.begin(); agent != agentsState.end(); agent++)
		{
			// if self
			if (agent->latest().value().GetID() == currentID)
			{
				self = *agent;
				break;
			}
		}

		// Fill others
		// TODO

		
		//itr->run(time, targetStates, neighborsStates);
	}
}