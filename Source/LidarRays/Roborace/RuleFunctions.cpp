#include "RuleFunctions.h"
#include "AgentTrajectory.h"

bool test_safety(const AgentTrajectory& targetStates, const AgentTrajectory& otherStates, const EnvironmentParameters& env, const Properties& automatonProperties) 
{
	// Avoid unused warning
	(void)automatonProperties;

	double selfX = targetStates.getTrajectory().latest().value().at("s");
	double otherX = otherStates.getTrajectory().latest().value().at("s");

	if (selfX > otherX)
		return true;
	else
		return (selfX + env.at("safety_dist") < otherX);
}