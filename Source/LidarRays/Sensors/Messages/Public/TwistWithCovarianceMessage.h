#pragma once

#include <array>
#include "TwistMessage.h"
#include <cereal/types/array.hpp>

template <class Archive>
class LIDARRAYS_API TwistWithCovarianceMessage : public MessageBase<Archive>
{

public:
	// Vectors are 3D, Covariances are 3*3 matrices
	TwistMessage<Archive> Twist;
	std::array<float, 36> Covariance;

	void serialize(Archive & ar) override
	{
		ar(Twist);
		ar(Covariance);
	}

	std::string ToString() override
	{
		return std::string();
	}
};