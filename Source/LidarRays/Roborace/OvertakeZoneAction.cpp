#include "OvertakeZoneAction.h"
#include "AgentTrajectory.h"
#include "Contestant.h"

const std::string OvertakeZoneAction::actionName = "OvertakeZone";

void OvertakeZoneAction::initRuleCategories()
{
	// Cannot overtake
	addRuleCategory("OvertakeZone");
}

bool OvertakeZoneAction::triggerCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	if (targetStates.parameter("current_zone_type") == 1)
		return true;

	return false;
}

bool OvertakeZoneAction::endCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	if (targetStates.parameter("current_zone_type") != 1)
		return true;

	return false;
}

bool OvertakeZoneAction::abortCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	return false;
}