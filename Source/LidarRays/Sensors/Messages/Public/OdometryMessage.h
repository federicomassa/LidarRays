#pragma once

#include <vector>
#include "PoseWithCovarianceMessage.h"
#include "TwistWithCovarianceMessage.h"
#include "MessageBase.h"

#pragma once

template <class Archive>
class OdometryMessage : public MessageBase<Archive>
{
public:
	float x, y, z;
	float vx, vy, vz;
	float yaw, pitch, roll;
	float yaw_rate, pitch_rate, roll_rate;

	std::array<float, 36> pose_covariance;
	std::array<float, 36> twist_covariance;

	void serialize(Archive & ar) override
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

//template <class Archive>
//class LIDARRAYS_API OdometryMessage : public MessageBase<Archive>
//{
//public:
//	float Timestamp;
//
//	// Vectors are 3D, Covariances are 3*3 matrices
//	
//	void serialize(Archive & ar) override
//	{
//		ar(Timestamp);
//		ar(PoseWithCovariance);
//		ar(TwistWithCovariance);
//	}
//
//	std::string ToString() override
//	{
//		return std::string();
//	}
//};