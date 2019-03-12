#include "RuleMonitor.h"
#include "AgentTrajectory.h"
#include <iostream>

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

void RuleMonitor::run(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates, const ActionManager* aMan) {
  /* error handling */
  if (rules == nullptr)
    LogFunctions::Error("RuleMonitor::run()", "Before run, use setRules method");
  if (aMan == nullptr)
	  LogFunctions::Error("RuleMonitor::run()", "Before run, use setActionManager method");
  
  for (auto tmpA = aMan->getHistory().begin(); tmpA != aMan->getHistory().end(); tmpA++)
  { 
      /* First see if the rule monitor has already registered this action */
      const ActionInfo* foundAction = nullptr;
	  std::pair<ActionInfo, std::vector<Rule> > processed;

      for (int i = 0; i < processedActions.size(); i++)
	{
	  processed = processedActions[i];
	  /* look for a match in ActionManager history and in RuleMonitor records */
	  if (processed.first.name == tmpA->name &&
	      processed.first.triggerTime == tmpA->triggerTime)
	    {
	      foundAction = &(*tmpA);
	      /* update end time */
	      processed.first.endTime = tmpA->endTime;
	      break;
	    }
	}

      /* if a match was not found */
      if (foundAction == nullptr)
		  registerNewAction(*tmpA);	  
      	
    }

  /* Now process actions taken by the register (processedActions list) */
  processActions(time, targetStates, neighborsStates);  
}

void RuleMonitor::processActions(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
  /* for each action recorded */
	for (auto& p : processedActions)
	{
  
		/* if we are past the end time of that action do not check it anymore */
		if (time > p.first.endTime && p.first.endTime != -1)
			continue;	

		for (auto r = p.second.begin(); r != p.second.end(); r++)
		{
	      r->evaluate(targetStates, neighborsStates, p.first.triggerTime, p.first.endTime, time, env_params, properties);		
	    }
	}
}

void RuleMonitor::registerNewAction(ActionInfo a)
{
  if (a.name == "")
    LogFunctions::Error("RuleMonitor::registerNewAction", "Function called with a null pointer argument");

  /* Rules will be processed by processAction method. */
  std::vector<Rule> actionRules = rules->createRulesList(a.ruleCategoryList);
  processedActions.insert(processedActions.begin(), std::make_pair(a, actionRules));

}

void RuleMonitor::UpdateEnvironmentParameters(double time, const EnvironmentParameters& params)
{
	env_params.insert(time, params);
}

void RuleMonitor::SetProperties(const Properties& prop)
{
	properties = prop;
}
