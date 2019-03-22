#include "RuleMonitor.h"
#include "AgentTrajectory.h"
#include "LogFunctions.h"
#include <iostream>
#include <sstream>

RuleMonitor::RuleMonitor()
{
	rules = 0;
}

RuleMonitor::~RuleMonitor()
{
}

void RuleMonitor::buildRules()
{
  if (rules)
    rules->build();
}

void RuleMonitor::run(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates, const ActionManager* aMan) 
{
  /* error handling */
  if (rules == nullptr)
    LogFunctions::Error("RuleMonitor::run()", "Before run, use setRules method");
  if (aMan == nullptr)
	  LogFunctions::Error("RuleMonitor::run()", "Before run, use setActionManager method");
  
  const auto& activeActions = aMan->getActiveActions();

  for (const auto& tmpA : activeActions)
  { 
	  /* Rules will be processed by processAction method. */
	  auto ruleCategories = tmpA.ruleCategoryList;
	  std::vector<Rule> actionRules = rules->createRulesList(ruleCategories);
	  for (auto& rule : actionRules)
	  {
		  rule.evaluate(targetStates, neighborsStates, tmpA.triggerTime, tmpA.endTime, time, env_params, properties);
	  }

	  processedActions.insert(time, std::make_pair(tmpA, actionRules));
  }
}

void RuleMonitor::SetProperties(const Properties& prop)
{
	properties = prop;
}

void RuleMonitor::UpdateEnvironmentParameters(double time, std::string key, double value)
{
	EnvironmentParameters par;
	par.AddEntry(key, value);

	UpdateEnvironmentParameters(time, par);
}


void RuleMonitor::UpdateEnvironmentParameters(double time, const EnvironmentParameters& param)
{
	if (env_params.size() == 0)
	{
		env_params.insert(time, param);
		return;
	}
	else
	{
		if (env_params.latest().time() < time)
		{
			env_params.insert(time, param);
			return;
		}
		else if (env_params.latest().time() == time)
		{
			EnvironmentParameters& latest = env_params.latest().value();
			for (const auto& itr : param)
			{
				if (latest.IsAvailable(itr.first))
					latest(itr.first) = itr.second;
				else
					latest.AddEntry(itr.first, itr.second);
			}
		}
		else
		{
			LogFunctions::Error("RuleMonitor::UpdateEnvironmentParameters", "trying to update in the past");
		}

	}
}