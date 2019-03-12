#include "SocialRules.h"
#include "AgentTrajectory.h"
#include "EnvironmentParameters.h"
#include "Properties.h"

Rule::Rule()
{
  lastCheckTime = -1;
  processed = false;
}

Rule::~Rule()
{
}

void Rule::evaluate(const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates, const double& triggerTime,
	const double& endTime, double currentTime, const TimedContainer<EnvironmentParameters>& environmentParameters, const Properties& properties)
{
  checkResult = false;
  
  /* The evaluation of the event is on the current state. See comments to template addRule!
     If we want to do something more complicated for specific rules we need to derive from rule our version of check() */
  for (auto e = eList.begin(); e != eList.end(); e++)
    {
	  bool evaluation = e->Evaluate(targetStates, neighborsStates, environmentParameters, properties);

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

SocialRules::SocialRules()
{

}

std::vector<Rule> SocialRules::createRulesList(const std::set<std::string>& ruleCategoryList)
{

	std::vector<Rule> actionRules;
  
  /* for each category */
	for (auto cat = ruleCategoryList.begin(); cat != ruleCategoryList.end(); cat++)
		{

		  /* for each rule composing the social rules */
		for (auto r = rList.begin(); r != rList.end(); r++)
			{
				if (r->category == *cat)
					actionRules.push_back(*r);
			}
		}

	return actionRules;
}

void SocialRules::setStateVars(std::set<std::string> states)
{
	state_vars = states;
}

std::set<std::string> SocialRules::getStateVars() const
{
	return state_vars;
}