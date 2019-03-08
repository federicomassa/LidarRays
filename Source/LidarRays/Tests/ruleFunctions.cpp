#include "ruleFunctions.h"

bool safety_1(const Agent& self, const Agent& other, const EnvironmentParameters& env, const Properties& automatonProperties)
{
	// Avoid unused warning
	automatonProperties;

	double selfX = self.GetState().at("x");
	double otherX = other.GetState().at("x");

	return (selfX + env.at("safety_dist") < otherX);
}