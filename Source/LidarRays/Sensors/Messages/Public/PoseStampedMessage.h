// =========== Twist message correspondent to ROS geometry_msgs/Twist message ============= //

#pragma once

#include <vector>
#include "PoseMessage.h"

template <class Archive>
class PoseStampedMessage : public MessageBase<Archive> // delete parent in ROS
{
public:

	float Timestamp;

	PoseMessage<Archive> Pose;

	void serialize(Archive & ar) override
	{
		ar(Timestamp);
		ar(Pose);
	}

	std::string ToString() override
	{
		return std::string();
	}
};