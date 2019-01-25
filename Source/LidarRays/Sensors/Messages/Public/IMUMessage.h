#pragma once

#include <vector>
#include <CoreMinimal.h> // delete in ROS
#include "MessageBase.h"
#include "IMUMessage.generated.h" // delete in ROS

UCLASS(Blueprintable, ClassGroup = (Messages))
class LIDARRAYS_API UIMUMessage : public UMessageBase // delete in ROS
{
	GENERATED_BODY() // delete in ROS

public:
	float timestamp;

	// Vectors are 3D, Covariances are 3*3 matrices
	std::vector<float> Orientation;
	std::vector<float> OrientationCov;

	std::vector<float> LinearAcceleration;
	std::vector<float> LinearAccelerationCov;

	std::vector<float> AngularVelocity;
	std::vector<float> AngularVelocityCov;

	template<class Archive>
	void serialize(Archive & ar)
	{
		ar(timestamp);
		ar(Orientation);
		ar(OrientationCov);
		ar(LinearAcceleration);
		ar(LinearAccelerationCov);
		ar(AngularVelocity);
		ar(AngularVelocityCov);
	}
};