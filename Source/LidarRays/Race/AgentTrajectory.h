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
	AgentParameters& getParameters() {
		return parameters;
	}

	const AgentParameters& getParameters() const {
		return parameters;
	}


	const TimedContainer<State>& getTrajectory() const;
	State& addState(double time, State s);
	void setCapacity(size_t);
	double& parameter(std::string name);
	const double& parameter(std::string name) const;
};

#endif