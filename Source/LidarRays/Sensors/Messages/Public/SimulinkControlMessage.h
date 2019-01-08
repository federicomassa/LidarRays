// =========== Twist message correspondent to ROS geometry_msgs/Twist message ============= //

#pragma once

#include <MessageBase.h>

template <class Archive>
class SimulinkControlMessage : public MessageBase<Archive>
{
public:
	// These should be vectors with 3 components (x,y,z)
	float VX, VY, VZ;

	// Velocities of roll, pitch, yaw
	float Rdot, Pdot, Ydot;

	void serialize(Archive & ar) override
	{
		ar(VX, VY, VZ);
		ar(Rdot, Pdot, Ydot);
	}

	std::string ToString() override
	{
		return std::string();
	}
};