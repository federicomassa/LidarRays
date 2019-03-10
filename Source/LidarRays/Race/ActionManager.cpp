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
  /* Add here any action that you want to be listened to with addListener. */
}

void ActionManager::run(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	for (auto a = listeners.begin(); a != listeners.end(); a++)
    {
      
      /* Checks the conditions of this action and update its status. */
      (*a)->listen(time, targetStates,neighborsStates);
      
      if ((*a)->info.status != INACTIVE)
	{
	  /* Add to history and reset action */
	  recordAction(*a);

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
}

void ActionManager::addListener(std::shared_ptr<Action> a)
{
  listeners.insert(a);
}

void ActionManager::recordAction(std::shared_ptr<Action> a)
{
	ActionInfo foundAction;
	bool saveAction = false;

	for (auto tmpA = history.begin(); tmpA != history.end(); tmpA++)
	{
		if (tmpA->name == a->name() &&
			tmpA->triggerTime == a->info.triggerTime)
		{
			foundAction = *tmpA;
			break;
		}
    }

  if (foundAction.name == "")
    {
      saveAction = true;
    }
  else
    {
      /* Status changed? Needs update */
	  if (a->info.status != foundAction.status)
	  {
		foundAction.status = a->info.status;
		foundAction.endTime = a->info.endTime;
	  }
	}
  
  if (saveAction)
	  history.insert(history.begin(), foundAction);
  
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
