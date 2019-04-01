#include "RuleFunctions.h"
#include "AgentTrajectory.h"

bool safety(const AgentTrajectory& targetStates, const AgentTrajectory& otherStates, const EnvironmentParameters& env, const Properties& automatonProperties) 
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

bool trigger(const AgentTrajectory& targetStates, const AgentTrajectory& otherStates, const EnvironmentParameters& env, const Properties& automatonProperties)
{
	// Avoid unused warning
	(void)automatonProperties;

	double selfX = targetStates.getTrajectory().latest().value().at("s");
	double otherX = otherStates.getTrajectory().latest().value().at("s");

	// This rule should only be applied to closest car in front
	if (otherX < selfX)
		return true;

	
	return false;
}