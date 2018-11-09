// =========== Twist message correspondent to ROS geometry_msgs/Twist message ============= //

#pragma once

#include <vector>
#include <CoreMinimal.h> // delete in ROS
#include "PoseMessage.generated.h" // delete in ROS

UCLASS(Blueprintable) // delete in ROS
class UPoseMessage : public UObject // delete parent in ROS
{
	GENERATED_BODY() // delete in ROS

public:
	float X;
	float Y;
	float Z;

	float Roll;
	float Pitch;
	float Yaw;

	template<class Archive>
	void serialize(Archive & ar)
	{
		ar(X);
		ar(Y);
		ar(Z);
		ar(Roll);
		ar(Pitch);
		ar(Yaw);
	}
};