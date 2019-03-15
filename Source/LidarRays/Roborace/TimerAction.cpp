#include "TimerAction.h"

const std::string TimerAction::actionName = "Timer";

void TimerAction::initRuleCategories()
{
  addRuleCategory("Safety");
}

bool TimerAction::triggerCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	// Silence warnings
	(void)targetStates;
	(void)neighborsStates;

	if (lastEndTime == -1)
		return true;
	else if (time > lastEndTime + 5.0)
		return true;

	return false;
}

bool TimerAction::endCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	// Silence warnings
	(void)targetStates;
	(void)neighborsStates;

	if (time > info.triggerTime + 10)
	{
		lastEndTime = time;
		return true;
	}

  return false;
}

bool TimerAction::abortCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	// Silence warnings
	(void)targetStates;
	(void)neighborsStates;
	(void)time;

  return false;
}
