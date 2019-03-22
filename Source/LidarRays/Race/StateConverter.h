/*
  AUTHOR = Federico Massa
  EMAIL = fedemassa91@gmail.com
  DESCRIPTION = This is an abstract class representing a general action of a vehicle, which can be
                a lane change, an overtake, etc. The specific actions inherit from this class.
 */

#ifndef STATE_CONVERTER_H
#define STATE_CONVERTER_H

#include "AgentParameters.h"

class State;

class StateConverter
{
public:
	// Convert state vars from world space to rule/action space. Optionally give additional environment parameters
	virtual State Convert(const State&, AgentParameters&) = 0;
	virtual ~StateConverter() {}
};

#endif