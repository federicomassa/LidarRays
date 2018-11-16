// =========== TwistStamped message correspondent to ROS geometry_msgs/Twist with timestamp message ============= //

#pragma once

#include <vector>
#include "TwistMessage.h"

template <class Archive>
class TwistStampedMessage : public MessageBase<Archive> // delete parent in ROS
{
public:

	float Timestamp;
	TwistMessage<Archive> Twist;
	
	void serialize(Archive & ar) override
	{
		ar(Timestamp);
		ar(Twist);
	}

	std::string ToString() override
	{
		return std::string();
	}

};