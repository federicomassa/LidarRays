#include "TriggerAttackAction.h"
#include "AgentTrajectory.h"

const std::string TriggerAttackAction::actionName = "TriggerAttack";

void TriggerAttackAction::initRuleCategories()
{
	// in seconds
	threshold_dT = 1.0;

	// Cannot overtake
	addRuleCategory("Trigger");
}

bool TriggerAttackAction::triggerCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	(void)time;

	// Check that this zone was not already triggered in the same lap
	if (lastZoneIDTriggered == targetStates.parameter("current_zone_id") && lastTriggerLap == targetStates.parameter("lap"))
		return false;


	double minDS = 1E9;

	if (targetStates.parameter("current_zone_type") != 1.0)
		return false;

	// Find closest opponent in front of you
	double myS = targetStates.getTrajectory().latest().value().at("s");
	const AgentTrajectory* candidateOpponent = nullptr;
	for (const auto& opponent : neighborsStates)
	{
		double opponentS = opponent.getTrajectory().latest().value().at("s");
		if (opponentS > myS && opponentS - myS < minDS)
		{
			minDS = opponentS - myS;
			candidateOpponent = &opponent;
		}
	}

	// distance in time from the opponent: dT = deltaS/v_chase_car
	double dT = minDS / targetStates.getTrajectory().latest().value().at("v");

	if (dT < threshold_dT)
	{
		last_s = targetStates.getTrajectory().latest().value().at("s");
		lastZoneIDTriggered = (int)targetStates.parameter("current_zone_id");
		lastTriggerLap = (int)targetStates.parameter("lap");
		return true;
	}

	return false;
}

bool TriggerAttackAction::endCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	(void)time;
	// Find closest opponent in front of you
	double minDS = 1E9;

	double myS = targetStates.getTrajectory().latest().value().at("s");
	const AgentTrajectory* candidateOpponent = nullptr;
	for (const auto& opponent : neighborsStates)
	{
		double opponentS = opponent.getTrajectory().latest().value().at("s");
		if (opponentS > myS && opponentS - myS < minDS)
		{
			minDS = opponentS - myS;
			candidateOpponent = &opponent;
		}
	}

	// distance in time from the opponent: dT = deltaS/v_chase_car
	double dT = minDS / targetStates.getTrajectory().latest().value().at("v");

	if (dT < threshold_dT)
	{
		dist_sum += myS - last_s;
		last_s = myS;
	}

	if (dist_sum > 0.8*targetStates.parameter("current_zone_length"))
	{
		dist_sum = 0;
		return true;
	}

	return false;
}

bool TriggerAttackAction::abortCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	(void)time;
	(void)neighborsStates;

	if (targetStates.parameter("current_zone_type") != 1.0)
	{
		dist_sum = 0;
		return true;
	}

	return false;
}
