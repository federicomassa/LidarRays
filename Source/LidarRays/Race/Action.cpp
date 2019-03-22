#include "Action.h"
#include "AgentTrajectory.h"
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

void Action::listen(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
  /* Init condition */
	if (info.status == INACTIVE)
	{
		if (triggerCondition(time, targetStates, neighborsStates))
		{
			info.status = TRIGGERED;
			info.triggerTime = time - getTriggerOffset();
			info.name = name();
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
			reset();
			return;
		}
		else if (abortCondition(time, targetStates, neighborsStates))
		{
			info.status = ABORTED;
			info.endTime = time - getAbortOffset();
			reset();
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

void Action::init()
{
	info.name = name();
	initRuleCategories();
}