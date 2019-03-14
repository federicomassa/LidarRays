#ifndef AGENT_TRAJECTORY_H
#define AGENT_TRAJECTORY_H

#include "State.h"
#include "TimedContainer.h"
#include "AgentParameters.h"
#include <string>

class AgentTrajectory
{
	std::string ID;
	AgentParameters parameters;
	TimedContainer<State> trajectory;

public:
	void setID(std::string id);
	std::string getID() const;
	void setParameters(AgentParameters params);
	const TimedContainer<State>& getTrajectory() const;
	void addState(double time, State s);
	void setCapacity(size_t);
};

#endif
