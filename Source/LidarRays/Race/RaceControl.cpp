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
		if (itr->ID() == ID)
			return;
	}

	contestants.push_back(Contestant());
	contestants.back().setID(ID);
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
		if (itr->ID() == a.GetID())
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

	found_itr->updateState(time, updated_state);

	// TODO manage maximum memory of trajectory
}

void RaceControl::Run(double time)
{
	std::stringstream ss;
	ss << time;

	std::string time_str;
	ss >> time_str;

	for (auto& contestant : contestants)
	{
		ActionManager* aMan = &contestant.actionManager();
		std::vector<AgentTrajectory> others;

		// Fill self containers
		for (const auto& opponent : contestants)
		{
			// if others
			if (opponent.ID() != contestant.ID()) 
			{
				others.push_back(opponent.trajectory());
			}
		}
		
		aMan->run(time, contestant.trajectory(), others);

		RuleMonitor* ruleMon = &contestant.ruleMonitor();
		ruleMon->run(time, contestant.trajectory(), others, aMan);
	}
}

void RaceControl::UpdateEnvironmentParameters(double time, const EnvironmentParameters& params)
{
	for (auto& contestant : contestants)
	{
		contestant.ruleMonitor().UpdateEnvironmentParameters(time, params);
	}
}

void RaceControl::SetProperties(const Properties& prop)
{
	for (auto& contestant : contestants)
	{
		contestant.ruleMonitor().SetProperties(prop);
	}
}
