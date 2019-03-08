#include "ActionManager.h"
#include "Timeseries.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <set>

ActionManager::ActionManager(const int& aID, const int& tID, const TimedContainer<Agent>* oStates, const TimedContainer<AgentVector>* mStates) : agentID(aID), targetID(tID),
																				     targetStates(oStates),
																				     neighborsStates(mStates)
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

void ActionManager::run(double time)
{
	for (auto a = listeners.begin(); a != listeners.end(); a++)
    {
      
      /* Checks the conditions of this action and update its status. */
      (*a)->listen(time);
      
      if ((*a)->info.status != INACTIVE)
	{
	  /* Add to history and reset action */
	  recordAction(*a);

	  if ((*a)->info.status != TRIGGERED)
	    resetAction(*a);
	  
	}
    }
  
}

void ActionManager::resetAction(Action* a)
{
  a->info.status = INACTIVE;
  a->info.triggerTime = -1;
  a->info.endTime = -1;
}

void ActionManager::addListener(Action* a)
{
  a->init(targetStates, neighborsStates);
  listeners.insert(a);
}

void ActionManager::recordAction(Action* a)
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
	for (auto l = listeners.begin(); l != listeners.end(); l++)
	{
		if (*l)
			delete (*l);
	}

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
