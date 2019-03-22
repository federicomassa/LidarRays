#include "ActionManager.h"
#include "AgentTrajectory.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <set>

ActionManager::ActionManager()
{}

ActionManager::~ActionManager()
{
  /* Take care of memory management */
  clearListeners();
  clearHistory();
}

void ActionManager::init()
{
}

void ActionManager::run(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	for (auto a = listeners.begin(); a != listeners.end(); a++)
    {
      
      /* Checks the conditions of this action and update its status. */
	if ((*a)->info.status == INACTIVE || (*a)->info.status == TRIGGERED)
		(*a)->listen(time, targetStates,neighborsStates);
      
	if ((*a)->info.status != INACTIVE)
		{
			/* Add to history and reset action */
			if (!findInHistory((*a)->info))
				recordAction((*a)->info);
			else
				updateAction(*a);

			// if ended or aborted
			if ((*a)->info.status != TRIGGERED)
				resetAction(*a);
		}
	}
  
}

void ActionManager::resetAction(std::shared_ptr<Action> a)
{
  a->info.status = INACTIVE;
  a->info.triggerTime = -1;
  a->info.endTime = -1;
  a->info.name = a->name();
}

void ActionManager::addListener(std::shared_ptr<Action> a)
{
	a->init();
	listeners.insert(a);
}

void ActionManager::recordAction(ActionInfo info)
{
	history.insert(history.begin(), info);  
}

void ActionManager::updateAction(std::shared_ptr<Action> a)
{
	for (auto& itr : history)
	{
		if (itr.name == a->name() &&
			itr.triggerTime == a->info.triggerTime)
		{
			itr.status = a->info.status;
			itr.endTime = a->info.endTime;
		}
	}
}

void ActionManager::clearListeners()
{
  listeners.clear();
}

void ActionManager::clearHistory()
{
  history.clear();
}

std::vector<ActionInfo> ActionManager::getActiveActions() const
{
	std::vector<ActionInfo> activeList;
  
	for (auto tmpA = history.begin(); tmpA != history.end(); tmpA++)
    {
		if (tmpA->status == TRIGGERED)
			activeList.insert(activeList.begin(), *tmpA);
    }
  
  return activeList;
}

bool ActionManager::findInHistory(ActionInfo info)
{
	bool found = false;

	for (auto tmpA = history.begin(); tmpA != history.end(); tmpA++)
	{
		if (tmpA->name == info.name &&
			tmpA->triggerTime == info.triggerTime)
		{
			found = true;
			break;
		}
	}

	return found;
}