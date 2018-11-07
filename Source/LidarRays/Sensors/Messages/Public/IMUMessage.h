#pragma once

#include <vector>
#include <CoreMinimal.h> // delete in ROS
#include "IMUMessage.generated.h" // delete in ROS

UCLASS(Blueprintable) // delete in ROS
class LIDARRAYS_API UIMUMessage : public UObject // delete in ROS
{
	GENERATED_BODY() // delete in ROS

public:
	float timestamp;

	// These must be same size and 1 to 1 correspondence
	std::vector<float> LinearAcceleration;
	std::vector<std::vector<float> > LinearAccelerationCov;

	std::vector<float> AngularVelocity;
	std::vector<std::vector<float> > AngularVelocityCov;

	template<class Archive>
	void serialize(Archive & ar)
	{
		ar(timestamp);
		ar(LinearAcceleration);
		ar(LinearAccelerationCov);
		ar(AngularVelocity);
		ar(AngularVelocityCov);
	}
};