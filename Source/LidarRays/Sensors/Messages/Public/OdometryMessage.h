#pragma once

#include <array>
#include "MessageBase.h"
#include "OdometryMessage.generated.h"

#pragma once

//UCLASS(Blueprintable, ClassGroup = (Messages))
USTRUCT(BlueprintType)
struct FOdometryMessage : public FMessageBase
{

	GENERATED_BODY()

	FOdometryMessage();
	~FOdometryMessage();

	float x, y, z;
	float vx, vy, vz;
	float yaw, pitch, roll;
	float yaw_rate, pitch_rate, roll_rate;

	std::array<float, 36> pose_covariance;
	std::array<float, 36> twist_covariance;

	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(x, y, z);
		ar(vx, vy, vz);
		ar(yaw, pitch, roll);
		ar(yaw_rate, pitch_rate, roll_rate);
		ar(pose_covariance);
		ar(twist_covariance);
	}

	std::string ToString() override
	{
		return std::string();
	}
};