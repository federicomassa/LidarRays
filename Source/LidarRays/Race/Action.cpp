#include "Action.h"
#include <iostream>
#include <sstream>
#include "RaceExceptions.h"

Action::Action()
{
}

void Action::init(const TimedContainer<Agent>* _targetStates, const TimedContainer<AgentVector>* _neighborsStates)
{
  targetStates = _targetStates;
  neighborsStates = _neighborsStates;
  initRuleCategories();
}

Action::Action(const Action& a)
{
  info.triggerTime = a.info.triggerTime;
  info.endTime = a.info.endTime;
  targetStates = a.targetStates;
  info.status = a.info.status;
  info.name = a.info.name;
  info.ruleCategoryList = a.info.ruleCategoryList;
}

void Action::listen(double time)
{
  /* Init condition */
	if (info.status == INACTIVE)
	{
		if (triggerCondition())
		{
			info.status = TRIGGERED;
			info.triggerTime = time - getTriggerOffset();
			return;
		}
	}
  /* Running */
	else if (info.status == TRIGGERED)
	{
		if (endCondition())
		{
			info.status = ENDED;
			info.endTime = time - getEndOffset();
			return;
		}
		else if (abortCondition())
		{
			info.status = ABORTED;
			info.endTime = time - getAbortOffset();
			return;
		}
    }

  /* Either ENDED or ABORTED */
	else
	{
		throw Race::TerminatedActionException(details().c_str());
	}  
}

std::string Action::details()
{
  std::string str;
  str = name() + '\t' + "from ";
  
  std::stringstream ss;
  std::string time;
  ss << info.triggerTime;
  ss >> time;
  ss.clear();
  
  str += time;
  
  ss << info.endTime;
  ss >> time;
  ss.clear();
  
  str += " to " + time + '\t' + toString(info.status);

  return str;
}

std::string toString(const ActionStatus& status)
{
  if (status == INACTIVE)
    return "INACTIVE";
  else if (status == TRIGGERED)
    return "TRIGGERED";
  else if (status == ENDED)
    return "ENDED";
  else if (status == ABORTED)
    return "ABORTED";

  return "";
}
