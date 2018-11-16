// =========== Twist message correspondent to ROS geometry_msgs/Twist message ============= //

#pragma once

#include "MessageBase.h"
#include <vector>

template <class Archive>
class PoseMessage : public MessageBase<Archive> 
{

public:
	float X;
	float Y;
	float Z;

	float Roll;
	float Pitch;
	float Yaw;

	void serialize(Archive & ar) override
	{
		ar(X);
		ar(Y);
		ar(Z);
		ar(Roll);
		ar(Pitch);
		ar(Yaw);
	}

	std::string ToString() override
	{
		return std::string();
	}
};