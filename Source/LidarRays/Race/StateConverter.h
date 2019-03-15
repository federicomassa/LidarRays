/*
  AUTHOR = Federico Massa
  EMAIL = fedemassa91@gmail.com
  DESCRIPTION = This is an abstract class representing a general action of a vehicle, which can be
                a lane change, an overtake, etc. The specific actions inherit from this class.
 */

#ifndef STATE_CONVERTER_H
#define STATE_CONVERTER_H

class State;

class StateConverter
{
public:
	virtual State Convert(const State&) = 0;
	virtual ~StateConverter() {}
};

#endif