#pragma once

#include <array>
#include "PoseMessage.h"
#include "MessageBase.h"

template <class Archive>
class LIDARRAYS_API PoseWithCovarianceMessage : public MessageBase<Archive> 
{

public:
	// Vectors are 3D, Covariances are 3*3 matrices
	PoseMessage<Archive> Pose;
	std::array<float, 36> Covariance;

	void serialize(Archive & ar) override
	{
		ar(Pose);
		ar(Covariance);
	}

	std::string ToString() override
	{
		return std::string();
	}
};