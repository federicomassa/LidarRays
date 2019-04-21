#include "Contestant.h"

State& Contestant::updateState(double time, State new_state)
{
	return traj.addState(time, new_state);
}

void Contestant::UpdateResults()
{
	results.ActionHistory = aMan.getHistory();

	const auto& processedActions = rMon.getProcessedActions();

	// Create vector without repetitions (more actions could have corresponded to the same rule)
	for (const auto& currentProcessedAction : processedActions)
	{
		std::set<Rule> processedRules;

		for (const auto& rule : currentProcessedAction.value().second)
		{
			if (processedRules.find(rule) == processedRules.end())
			{
				processedRules.insert(rule);
			}

		}

		//results.RulesHistory.insert(currentProcessedAction.time(), processedRules);
	}
}
