#pragma once

#include <array>
#include <CoreMinimal.h> // delete in ROS
#include "TwistMessage.h"
#include "TwistWithCovarianceMessage.generated.h" // delete in ROS

UCLASS(Blueprintable) // delete in ROS
class LIDARRAYS_API UTwistWithCovarianceMessage : public UObject // delete in ROS
{
	GENERATED_BODY() // delete in ROS

public:
	// Vectors are 3D, Covariances are 3*3 matrices
	UTwistMessage* Twist;
	std::array<float, 36> Covariance;

	template<class Archive>
	void serialize(Archive & ar)
	{
		ar(*Twist);
		ar(Covariance);
	}
};