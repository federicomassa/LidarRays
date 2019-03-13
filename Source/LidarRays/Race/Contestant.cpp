#include "Contestant.h"

void Contestant::updateState(double time, State new_state)
{
	traj.addState(time, new_state);
}
