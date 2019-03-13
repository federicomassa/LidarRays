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

void RuleMonitor::UpdateEnvironmentParameters(double time, const EnvironmentParameters& params)
{
	env_params.insert(time, params);
}

void RuleMonitor::SetProperties(const Properties& prop)
{
	properties = prop;
}
