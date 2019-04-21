// Fill out your copyright notice in the Description page of Project Settings.

#include "RaceControl.h"
#include "LogFunctions.h"
#include "Action.h"
#include "SocialRules.h"
#include "RaceExceptions.h"
#include "StateConverter.h"

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
	stateConverter = nullptr;
}

State& RaceControl::Update(double time, Agent a)
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

	if (conversionFcn == nullptr && stateConverter == nullptr)
	{
		throw UninitializedException("StateConversionFcn or StateConverter");
	}

	State updated_state;

	// Additional environment parameters
	AgentParameters pars;

	if (conversionFcn)
		updated_state = conversionFcn(a.GetState());
	else if (stateConverter)
		updated_state = stateConverter->Convert(a.GetState(), pars);

	State& new_state = found_itr->updateState(time, updated_state);
	UpdateContestantParameters(found_itr->ID(), pars);

	return new_state;
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

		contestant.UpdateResults();
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

void RaceControl::setTrajectoryCapacity(size_t c)
{
	for (auto& contestant : contestants)
	{
		contestant.trajectory().setCapacity(c);
	}
}

void RaceControl::SetStateConverter(StateConverter* converter)
{
	stateConverter = std::shared_ptr<StateConverter>(converter);
	conversionFcn = nullptr;
}

void RaceControl::UpdateContestantParameters(std::string ID, const AgentParameters& parameters)
{
	Contestant* c = GetContestant(ID);

	LogFunctions::Require(c != nullptr, "RaceControl::UpdateContestantParameters", "Contestant not found");

	AgentParameters& current_pars = c->trajectory().getParameters();

	for (const auto& param : parameters)
	{
		if (current_pars.IsAvailable(param.first))
		{
			current_pars(param.first) = param.second;
		}
		else
		{
			current_pars.AddEntry(param.first, param.second);
		}
	}

}

Contestant* RaceControl::GetContestant(std::string ID)
{
	Contestant* found = nullptr;

	for (auto itr = contestants.begin(); itr != contestants.end(); itr++)
	{
		if (itr->ID() == ID)
		{
			found = &(*itr);
			break;
		}
	}

	return found;
}

