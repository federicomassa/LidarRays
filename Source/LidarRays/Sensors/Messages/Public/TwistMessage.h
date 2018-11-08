// =========== Twist message correspondent to ROS geometry_msgs/Twist message ============= //

#pragma once

#include <vector>
#include <CoreMinimal.h> // delete in ROS
#include "TwistMessage.generated.h" // delete in ROS

UCLASS(Blueprintable) // delete in ROS
class UTwistMessage : public UObject // delete parent in ROS
{
	GENERATED_BODY() // delete in ROS

public:

	float Timestamp;

	// These should be vectors with 3 components (x,y,z)
	std::vector<float> Linear;
	std::vector<float> Angular;

	template<class Archive>
	void serialize(Archive & ar)
	{
		ar(Timestamp);
		ar(Linear);
		ar(Angular);
	}

	// delete in ROS
	UFUNCTION(BlueprintCallable)
	FString ToString();

	// delete in ROS
	UFUNCTION(BlueprintCallable, Category = Controller) 
	float GetTimeStamp() const
	{
		return Timestamp;
	}

	UFUNCTION(BlueprintCallable, Category = Controller)
	FVector GetLinear() const
	{
		if (Linear.size() != 3) return FVector();

		return FVector(Linear[0], Linear[1], Linear[2]);
	}

	UFUNCTION(BlueprintCallable, Category = Controller)
	FVector GetAngular() const
	{
		if (Angular.size() != 3) return FVector();

		return FVector(Angular[0], Angular[1], Angular[2]);
	}

};