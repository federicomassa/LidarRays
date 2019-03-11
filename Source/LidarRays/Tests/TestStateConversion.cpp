#include "TestStateConversion.h"

State UnrealToUnicycle(State s_in)
{
	State s_out;
	s_out.AddStateVariable("x");
	s_out.AddStateVariable("y");
	s_out.AddStateVariable("theta");

	s_out("x") = s_in("x")*0.01;
	s_out("y") = -s_in("y")*0.01;
	s_out("theta") = s_in("yaw")*3.14159 / 180.0;

	return s_out;
}