// =========== TwistStamped message correspondent to ROS geometry_msgs/Twist with timestamp message ============= //

#pragma once

#include <vector>
#include <CoreMinimal.h> // delete in ROS
#include "TwistMessage.h"
#include "TwistStampedMessage.generated.h" // delete in ROS

UCLASS(Blueprintable) // delete in ROS
class UTwistStampedMessage : public UObject // delete parent in ROS
{
	GENERATED_BODY() // delete in ROS

public:

	float Timestamp;
	UTwistMessage* Twist;
	
	template<class Archive>
	void serialize(Archive & ar)
	{
		ar(Timestamp);
		if (Twist)
			ar(*Twist);
		else
			UE_LOG(LogTemp, ERROR, TEXT("UTwistStampedMessage::serialize - Twist message not built."));
	}

	// delete in ROS
	UFUNCTION(BlueprintCallable)
	FString ToString();

	// delete in ROS
	UFUNCTION(BlueprintCallable, Category = Controller) 
	float GetTimestamp() const
	{
		return Timestamp;
	}

	UFUNCTION(BlueprintCallable, Category = Controller)
	FVector GetLinear() const
	{
		if (Twist) return Twist->GetLinear();

		return FVector();
	}

	UFUNCTION(BlueprintCallable, Category = Controller)
	FVector GetAngular() const
	{
		if (Twist) return Twist->GetAngular();
		return FVector();
	}

};