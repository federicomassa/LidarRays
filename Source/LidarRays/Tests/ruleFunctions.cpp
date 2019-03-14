#include "ruleFunctions.h"
#include "AgentTrajectory.h"

bool safety_1(const AgentTrajectory& targetStates, const AgentTrajectory& otherStates, const EnvironmentParameters& env, const Properties& automatonProperties) 
{
	// Avoid unused warning
	(void)automatonProperties;

	double selfX = targetStates.getTrajectory().latest().value().at("x");
	double otherX = otherStates.getTrajectory().latest().value().at("x");

	if (selfX > otherX)
		return true;
	else
		return (selfX + env.at("safety_dist") < otherX);
}