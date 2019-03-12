#include "TestAction.h"

const std::string TestAction::actionName = "TEST";

void TestAction::initRuleCategories()
{
  addRuleCategory("TestCategory");
}

bool TestAction::triggerCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	// Silence warnings
	(void)targetStates;
	(void)neighborsStates;

	return true;
}

bool TestAction::endCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	// Silence warnings
	(void)targetStates;
	(void)neighborsStates;

	if (time > info.triggerTime + 10)
		return true;

  return false;
}

bool TestAction::abortCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	// Silence warnings
	(void)targetStates;
	(void)neighborsStates;
	(void)time;

  return false;
}
