#pragma once

#include <array>
#include <CoreMinimal.h> // delete in ROS
#include "MessageBase.h"
#include "Point3D.h"
#include "IMUMessage.generated.h" // delete in ROS

//UCLASS(Blueprintable, ClassGroup = (Messages))
USTRUCT(BlueprintType)
struct LIDARRAYS_API FIMUMessage : public FMessageBase // delete in ROS
{
	GENERATED_BODY() // delete in ROS

	float timestamp;

	// Vectors are 3D, Covariances are 3*3 matrices
	Point3D Orientation;
	std::array<float, 9> OrientationCov;

	Point3D LinearAcceleration;
	std::array<float, 9> LinearAccelerationCov;

	Point3D AngularVelocity;
	std::array<float, 9> AngularVelocityCov;

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