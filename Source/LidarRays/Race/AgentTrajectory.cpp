#include "AgentTrajectory.h"

void AgentTrajectory::setID(std::string id)
{
	ID = id;
}

std::string AgentTrajectory::getID() const
{
	return ID;
}


void AgentTrajectory::setParameters(AgentParameters params)
{
	parameters = params;
}

const TimedContainer<State>& AgentTrajectory::getTrajectory() const
{
	return trajectory;
}

void AgentTrajectory::addState(double time, State s)
{
	if (trajectory.size() != 0)
		CheckConsistency("AgentTrajectory::addState", s, trajectory.latest().value());

	trajectory.insert(time, s);
}