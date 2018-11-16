// =========== Twist message correspondent to ROS geometry_msgs/Twist message ============= //

#pragma once

#include <vector>
#include <cereal/types/vector.hpp>
#include <MessageBase.h>

template <class Archive>
class TwistMessage : public MessageBase<Archive>
{
public:
	// These should be vectors with 3 components (x,y,z)
	float Timestamp;
	std::vector<float> Linear;
	std::vector<float> Angular;

	void serialize(Archive & ar) override
	{
		ar(Timestamp);
		ar(Linear);
		ar(Angular);
	}

	std::string ToString() override
	{
		return std::string();
	}
};