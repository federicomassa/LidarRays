#include "OvertakeAction.h"
#include "AgentTrajectory.h"
#include "Contestant.h"

const std::string OvertakeAction::actionName = "Overtake";

void OvertakeAction::initRuleCategories()
{
	// Cannot overtake
	addRuleCategory("Overtake");
}

bool OvertakeAction::triggerCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	// TODO more than 2 contestants case

	(void)time;

	const auto* closest_behind = GetClosestBehind(targetStates, neighborsStates);
	const auto* closest_ahead = GetClosestAhead(targetStates, neighborsStates);

	// Only 1 contestant
	if (closest_behind == nullptr && closest_ahead == nullptr)
		return false;

	if (closest_ahead)
		closest_ahead_id = closest_ahead->getID();
	if (closest_behind)
		closest_behind_id = closest_behind->getID();

	if (closest_ahead == nullptr)
	{
		if (closest_ahead_id != closest_behind_id)
			return false;
		else
			return true;
	}

	return false;
}

bool OvertakeAction::endCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	(void)time;

	const auto* closest_behind = GetClosestBehind(targetStates, neighborsStates);

	if (closest_behind == nullptr)
		return false;

	double current_ds = targetStates.getTrajectory().latest().value()("s") - closest_behind->getTrajectory().latest().value()("s");

	if (current_ds > safety)
		return true;

	return false;
}

bool OvertakeAction::abortCondition(double time, const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	(void)time;
	(void)targetStates;
	(void)neighborsStates;

	const auto* closest_behind = GetClosestBehind(targetStates, neighborsStates);

	if (closest_behind == nullptr)
		return true;

	return false;
}

void OvertakeAction::reset()
{
	closest_ahead_id = "";
	closest_behind_id = "";
}

const AgentTrajectory* OvertakeAction::GetClosestAhead(const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	double minDS = 1E9;

	// Find closest opponent in front of you
	const AgentTrajectory* candidate = nullptr;

	double myS = targetStates.getTrajectory().latest().value().at("s");
	for (const auto& opponent : neighborsStates)
	{
		double opponentS = opponent.getTrajectory().latest().value().at("s");
		if (opponentS > myS && opponentS - myS < minDS)
		{
			minDS = opponentS - myS;
			candidate = &opponent;
		}
	}

	return candidate;
}

const AgentTrajectory* OvertakeAction::GetClosestBehind(const AgentTrajectory& targetStates, const std::vector<AgentTrajectory>& neighborsStates)
{
	double minDS = 1E9;

	// Find closest opponent behind you
	const AgentTrajectory* candidate = nullptr;

	double myS = targetStates.getTrajectory().latest().value().at("s");
	for (const auto& opponent : neighborsStates)
	{
		double opponentS = opponent.getTrajectory().latest().value().at("s");
		if (opponentS < myS && myS - opponentS < minDS)
		{
			minDS = myS - opponentS;
			candidate = &opponent;
		}
	}

	return candidate;
}
