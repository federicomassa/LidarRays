#pragma once

#include <vector>
#include "PoseWithCovarianceMessage.h"
#include "TwistWithCovarianceMessage.h"
#include "MessageBase.h"

template <class Archive>
class LIDARRAYS_API OdometryMessage : public MessageBase<Archive>
{
public:
	float Timestamp;

	// Vectors are 3D, Covariances are 3*3 matrices
	PoseWithCovarianceMessage<Archive> PoseWithCovariance;
	TwistWithCovarianceMessage<Archive> TwistWithCovariance;

	void serialize(Archive & ar) override
	{
		ar(Timestamp);
		ar(PoseWithCovariance);
		ar(TwistWithCovariance);
	}

	std::string ToString() override
	{
		return std::string();
	}
};