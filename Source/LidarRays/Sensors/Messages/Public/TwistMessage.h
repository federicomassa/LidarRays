// =========== Twist message correspondent to ROS geometry_msgs/Twist message ============= //

#pragma once

#include <array>
#include <cereal/types/array.hpp>
#include <MessageBase.h>

template <class Archive>
class TwistMessage : public MessageBase<Archive>
{
public:
	// These should be vectors with 3 components (x,y,z)
	std::array<float, 3> Linear;
	std::array<float, 3> Angular;

	void serialize(Archive & ar) override
	{
		ar(Linear);
		ar(Angular);
	}

	std::string ToString() override
	{
		return std::string();
	}
};