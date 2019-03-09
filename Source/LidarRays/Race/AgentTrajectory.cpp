#include "AgentTrajectory.h"

void AgentTrajectory::setID(std::string id)
{
	ID = id;
}

void AgentTrajectory::setParameters(AgentParameters params)
{
	parameters = params;
}

const TimedContainer<State>& AgentTrajectory::getTrajectory() const
{
	return trajectory;
}