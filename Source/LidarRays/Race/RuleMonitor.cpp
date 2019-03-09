#include "RuleMonitor.h"
#include "AgentTrajectory.h"
#include <iostream>

RuleMonitor::RuleMonitor(const ActionManager& aM): aMan(aM)
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

void RuleMonitor::run(double time, const AgentTrajectory& targetStates, const std::list<AgentTrajectory>& neighborsStates) {
  /* error handling */
  if (rules == 0)
    LogFunctions::Error("RuleMonitor::run()", "Before checking, use setRules() method");
  
  for (auto tmpA = aMan.getHistory().begin(); tmpA != aMan.getHistory().end(); tmpA++)
  { 
      /* First see if the rule monitor has already registered this action */
      ActionInfo foundAction;
	  std::pair<ActionInfo, std::set<Rule> > processed;

      for (int i = 0; i < processedActions.size(); i++)
	{
	  processed = processedActions[i];
	  /* look for a match in ActionManager history and in RuleMonitor records */
	  if (processed.first.name == tmpA->name &&
	      processed.first.triggerTime == tmpA->triggerTime)
	    {
	      foundAction = *tmpA;
	      /* update end time */
	      processed.first.endTime = tmpA->endTime;
	      break;
	    }
	}

      /* if a match was not found */
      if (foundAction.name == "")
		  registerNewAction(*tmpA);	  
      	
    }

  /* Now process actions taken by the register (processedActions list) */
  processActions(time, targetStates, neighborsStates);  
}

void RuleMonitor::processActions(double time, const AgentTrajectory& targetStates, const std::list<AgentTrajectory>& neighborsStates)
{
  
  std::pair<ActionInfo, std::set<Rule> > p;

  /* list containing rules processed during this run. 
   It helps to avoid checking several time the same rule
  (different actions might share a rule) */
  processedRules.clear();
  
  /* for each action recorded */
  for (int i = 0; i < processedActions.size(); i++)
    {
      p = processedActions[i];
      
      /* if we are past the end time of that action do not check it anymore */
      if (time > p.first.endTime && p.first.endTime != -1)
		continue;

      for (auto r_itr = p.second.begin(); r_itr != p.second.end(); r_itr++)
	{
	  Rule r = *r_itr;

	  /* check if rule has already been processed */
	  bool ruleFound = false;
	  
	  for (auto procR = processedRules.begin(); procR != processedRules.end(); procR++)
	    {
	      if (*procR == r)
			{
				ruleFound = true;
				break;
			}
	    }

	  if (!r.isProcessed() && !ruleFound)
	    {
	      r.evaluate(targetStates, neighborsStates, p.first.triggerTime, p.first.endTime, time);
	      processedRules.insert(processedRules.begin(), r);
		
	    }
	}
    }
}

void RuleMonitor::registerNewAction(ActionInfo a)
{
  if (a.name == "")
    LogFunctions::Error("RuleMonitor::registerNewAction", "Function called with a null pointer argument");

  /* Rules will be processed by processAction method. */
  std::set<Rule> actionRules = rules->createRulesList(a.ruleCategoryList);
  processedActions.insert(processedActions.begin(), std::make_pair(a, actionRules));

}
