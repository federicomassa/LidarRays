
#ifndef RULEFUNCTIONS_H
#define RULEFUNCTIONS_H

#include <cmath>
#include "Agent.h"
#include "EnvironmentParameters.h"
#include "Properties.h"
#include "StateRegion.h"

bool safety_1(const Agent& self, const Agent& other, const EnvironmentParameters& env, const Properties& automatonProperties);

#endif
