#pragma once

#include <vector>
#include <array>
#include <CoreMinimal.h> // delete in ROS
#include "cereal/types/vector.hpp"
#include "cereal/types/array.hpp"
#include "MessageBase.h"
#include "Point3D.h"
#include "LidarMessage.generated.h" // delete in ROS

UCLASS(Blueprintable, ClassGroup = (Messages))
class ULidarMessage : public UMessageBase
{
	GENERATED_BODY() // delete in ROS

public:
	float timestamp;

	// These must be same size and 1 to 1 correspondence
	std::vector<Point3D> Points;

	template<class Archive>
	void serialize(Archive & ar)
	{
		ar(timestamp);
		ar(Points);
	}

	ULidarMessage();
	void Empty();

	UFUNCTION(BlueprintCallable, Category = Lidar) // delete in ROS
	float GetTimeStamp()
	{
		return timestamp;
	}
};