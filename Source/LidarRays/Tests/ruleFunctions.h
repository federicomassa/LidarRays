
#ifndef RULEFUNCTIONS_H
#define RULEFUNCTIONS_H

#include <cmath>
#include "Agent.h"
#include "EnvironmentParameters.h"
#include "Properties.h"
#include "StateRegion.h"

class AgentTrajectory;

bool safety_1(const AgentTrajectory& targetStates, const AgentTrajectory& otherStates, const EnvironmentParameters& env, const Properties& automatonProperties);

#endif
