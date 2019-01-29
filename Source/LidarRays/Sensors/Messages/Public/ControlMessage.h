// =========== Twist message correspondent to ROS geometry_msgs/Twist message ============= //

#pragma once

#include <MessageBase.h>
#include "ControlMessage.generated.h"

//UCLASS(Blueprintable, ClassGroup = (Messages))
USTRUCT(BlueprintType)
struct FControlMessage : public FMessageBase
{
	GENERATED_BODY()

	// These should be vectors with 3 components (x,y,z)
	float VX, VY, VZ;

	// Velocities of roll, pitch, yaw
	float Rdot, Pdot, Ydot;

	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(VX, VY, VZ);
		ar(Rdot, Pdot, Ydot);
	}

	std::string ToString() override
	{
		return std::string();
	}
};