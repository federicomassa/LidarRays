#pragma once

#include <vector>
#include <CoreMinimal.h> // delete in ROS
#include "PoseWithCovarianceMessage.h"
#include "TwistWithCovarianceMessage.h"
#include "OdometryMessage.generated.h" // delete in ROS

UCLASS(Blueprintable) // delete in ROS
class LIDARRAYS_API UOdometryMessage : public UObject // delete in ROS
{
	GENERATED_BODY() // delete in ROS

public:
	float Timestamp;

	// Vectors are 3D, Covariances are 3*3 matrices
	UPoseWithCovarianceMessage* PoseWithCovariance;
	UTwistWithCovarianceMessage* TwistWithCovariance;

	template<class Archive>
	void serialize(Archive & ar)
	{
		ar(Timestamp);
		ar(*PoseWithCovariance);
		ar(*TwistWithCovariance);
	}
};