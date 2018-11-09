#pragma once

#include <array>
#include <CoreMinimal.h> // delete in ROS
#include "PoseMessage.h"
#include "PoseWithCovarianceMessage.generated.h" // delete in ROS

UCLASS(Blueprintable) // delete in ROS
class LIDARRAYS_API UPoseWithCovarianceMessage : public UObject // delete in ROS
{
	GENERATED_BODY() // delete in ROS

public:
	// Vectors are 3D, Covariances are 3*3 matrices
	UPoseMessage* Pose;
	std::array<float, 36> Covariance;

	template<class Archive>
	void serialize(Archive & ar)
	{
		ar(*Pose);
		ar(Covariance);
	}
};