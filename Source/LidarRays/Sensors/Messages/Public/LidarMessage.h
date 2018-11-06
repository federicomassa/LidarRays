#pragma once

#include <vector>
#include <CoreMinimal.h> // delete in ROS
#include "LidarMessage.generated.h" // delete in ROS

UCLASS(Blueprintable) // delete in ROS
class ULidarMessage : public UObject // delete in ROS
{
	GENERATED_BODY() // delete in ROS

public:
	float timestamp;

	// These must be same size and 1 to 1 correspondence
	std::vector<float> PointsX;
	std::vector<float> PointsY;
	std::vector<float> PointsZ;

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