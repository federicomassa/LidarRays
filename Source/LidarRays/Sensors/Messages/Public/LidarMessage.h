#pragma once

#include <vector>
#include <CoreMinimal.h> // delete in ROS
#include "LidarMessage.generated.h" // delete in ROS

UCLASS(Blueprintable) // delete in ROS
class ULidarMessage : public UObject // delete in ROS
{
	GENERATED_BODY() // delete in ROS

public:
	struct Point
	{
		float X;
		float Y;
		float Z;

		Point(float x, float y, float z);
	};

public:
	float timestamp;
	std::vector<Point> Data;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & timestamp;
		for (int i = 0; i < Data.size(); i++)
		{
			ar & Data[i].X;
			ar & Data[i].Y;
			ar & Data[i].Z;
		}
	}

	ULidarMessage();
	void Empty();

	UFUNCTION(BlueprintCallable, Category = Lidar)
	float GetTimeStamp()
	{
		return timestamp;
	}
};