#pragma once

#include <array>
#include <CoreMinimal.h> // delete in ROS
#include "cereal/types/array.hpp"
#include "LidarMessage.generated.h" // delete in ROS

constexpr int Npoints = 9600;

UCLASS(Blueprintable) // delete in ROS
class ULidarMessage : public UObject // delete in ROS
{
	GENERATED_BODY() // delete in ROS

public:
	float timestamp;

	// These must be same size and 1 to 1 correspondence
	std::array<float, Npoints> PointsX;
	std::array<float, Npoints> PointsY;
	std::array<float, Npoints> PointsZ;

	template<class Archive>
	void serialize(Archive & ar)
	{
		ar(timestamp);
		ar(PointsX);
		ar(PointsY);
		ar(PointsZ);
	}

	ULidarMessage() {}
};