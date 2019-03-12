// Fill out your copyright notice in the Description page of Project Settings.

#include "RaceControl.h"
#include "LogFunctions.h"
#include "Action.h"
#include "SocialRules.h"
#include "RaceExceptions.h"

#include <sstream>


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
	ruleMonitors.push_back(RuleMonitor());

	LogFunctions::Require(actionManagers.size() == ruleMonitors.size(), "RaceControl::RegisterContestant", "Action managers and rule monitors must be same size and in sync");
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
	std::stringstream ss;
	ss << time;

	std::string time_str;
	ss >> time_str;

	for (int i = 0; i < actionManagers.size(); i++)
	{
		ActionManager* aMan = &actionManagers[i];
		std::string currentID = aMan->getID();
		AgentTrajectory self;
		std::vector<AgentTrajectory> others;

		// Fill self containers
		for (auto agent = contestants.begin(); agent != contestants.end(); agent++)
		{
			// if self
			if (agent->getID() == currentID) 
			{
				self = *agent;
			}
			else
			{
				others.push_back(*agent);
			}
		}
		
		aMan->run(time, self, others);

		RuleMonitor* ruleMon = &ruleMonitors[i];
		ruleMon->run(time, self, others, aMan);

		auto action_rules = ruleMon->getProcessedActions();
		for (const auto& action_rule : action_rules)
		{
			const std::vector<Rule>& rules = action_rule.second;

			for (const auto& rule : rules)
			{
				bool result = rule.getResult();

				if (result)
					LogFunctions::Info("RaceControl::Run", (std::string("Time: ") + time_str + " --- Rule " + rule.getName() + " was OK").c_str());
				else
					LogFunctions::Info("RaceControl::Run", (std::string("Time: ") + time_str + " --- Rule " + rule.getName() + " was VIOLATED").c_str());
			}
		}
	}
}

void RaceControl::UpdateEnvironmentParameters(double time, const EnvironmentParameters& params)
{
	for (auto& itr : ruleMonitors)
	{
		itr.UpdateEnvironmentParameters(time, params);
	}
}

void RaceControl::SetProperties(const Properties& prop)
{
	for (auto itr : ruleMonitors)
	{
		itr.SetProperties(prop);
	}
}
