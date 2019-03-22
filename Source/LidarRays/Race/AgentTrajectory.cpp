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

State& AgentTrajectory::addState(double time, State s)
{
	if (trajectory.size() != 0)
		CheckConsistency("AgentTrajectory::addState", s, trajectory.latest().value());

	if (trajectory.size() == 0)
	{
		trajectory.insert(time, s);
	}
	else
	{
		// if same time, just update its value
		if (trajectory.latest().time() == time)
		{
			trajectory.latest().value() = s;
		}
		else
			trajectory.insert(time, s);
	}
	
	return trajectory.latest().value();
}

void AgentTrajectory::setCapacity(size_t c)
{
	trajectory.setCapacity(c);
}

double& AgentTrajectory::parameter(std::string name)
{
	return parameters(name);
}

const double& AgentTrajectory::parameter(std::string name) const
{
	return parameters(name);
}