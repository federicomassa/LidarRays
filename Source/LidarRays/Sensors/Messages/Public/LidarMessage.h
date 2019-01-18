#pragma once

#include <vector>
#include <array>
#include <CoreMinimal.h> // delete in ROS
#include "cereal/types/vector.hpp"
#include "cereal/types/array.hpp"
#include "LidarMessage.generated.h" // delete in ROS

UCLASS(Blueprintable) // delete in ROS
class ULidarMessage : public UObject // delete in ROS
{
	GENERATED_BODY() // delete in ROS

public:
	float timestamp;

	// These must be same size and 1 to 1 correspondence
	std::array<float, 9600> PointsX;
	std::array<float, 9600> PointsY;
	std::array<float, 9600> PointsZ;

	template<class Archive>
	void serialize(Archive & ar)
	{
		ar(timestamp);
		ar(PointsX);
		ar(PointsY);
		ar(PointsZ);
	}

	ULidarMessage();
	void Empty();

	UFUNCTION(BlueprintCallable, Category = Lidar) // delete in ROS
	float GetTimeStamp()
	{
		return timestamp;
	}
};