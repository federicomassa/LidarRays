#include "SocialRules.h"

int Rule::count = 0;

Rule::Rule()
{
  lastCheckTime = -1;
  processed = false;
  unique_id = count;
  count++;
}

Rule::~Rule()
{
	count--;
}

void Rule::evaluate(const TimedContainer<Agent>& targetStates, const TimedContainer<AgentVector>& neighStates, const double& triggerTime,
	const double& endTime, double currentTime)
{
  checkResult = false;
  
  /* The evaluation of the event is on the current state. See comments to template addRule!
     If we want to do something more complicated for specific rules we need to derive from rule our version of check() */
  for (auto e = eList.begin(); e != eList.end(); e++)
    {
	  TimedContainer<EnvironmentParameters> env_param;
	  Properties properties; 
	  bool evaluation = e->Evaluate(targetStates, neighStates, env_param, properties);

	  checkResult = checkResult || evaluation;
    }
  
  lastCheckTime = currentTime;
  
  updateProcessStatus(triggerTime, endTime);
  
}


void Rule::init(const std::string& cat, const std::set<Event>& eL, const std::string& n, const CheckMode& m)
{
  category = cat;
  eList = eL;
  name = n;
  mode = m;
}


void Rule::updateProcessStatus(const double& triggerTime, const double& endTime)
{
  switch(mode)
    {
      /* evaluation at trigger time */
    case TRIGGER:
      if (lastCheckTime >= triggerTime)
	processed = true;
      break;
      
      /* continuous evaluation until the end of the action */
    case CONTINUOUS:
      if (lastCheckTime >= endTime && endTime != -1)
	processed = true;
      break;
      
    default:
      LogFunctions::Error("Rule::isProcessed", "This rule's CheckMode was not recognized"); 
    }

}

bool Rule::isProcessed() const {return processed;}

std::set<Rule> SocialRules::createRulesList(const std::set<std::string>& ruleCategoryList)
{

	std::set<Rule> actionRules;
  
  /* for each category */
	for (auto cat = ruleCategoryList.begin(); cat != ruleCategoryList.end(); cat++)
		{

		  /* for each rule composing the social rules */
		for (auto r = rList.begin(); r != rList.end(); r++)
			{
				if (r->category == *cat)
					actionRules.insert(*r);
			}
		}

	return actionRules;
}
