#include "Action.h"
#include <iostream>
#include <sstream>
#include "RaceExceptions.h"

Action::Action()
{
}

Action::Action(const Action& a)
{
  info.triggerTime = a.info.triggerTime;
  info.endTime = a.info.endTime;
  info.status = a.info.status;
  info.name = a.info.name;
  info.ruleCategoryList = a.info.ruleCategoryList;
}

void Action::listen(double time, const TimedContainer<Agent>* targetStates, const TimedContainer<AgentVector>* neighborsStates)
{
  /* Init condition */
	if (info.status == INACTIVE)
	{
		if (triggerCondition(time, targetStates, neighborsStates))
		{
			info.status = TRIGGERED;
			info.triggerTime = time - getTriggerOffset();
			return;
		}
	}
  /* Running */
	else if (info.status == TRIGGERED)
	{
		if (endCondition(time, targetStates, neighborsStates))
		{
			info.status = ENDED;
			info.endTime = time - getEndOffset();
			return;
		}
		else if (abortCondition(time, targetStates, neighborsStates))
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
