#pragma once

#include <vector>
#include "PoseWithCovarianceMessage.h"
#include "TwistWithCovarianceMessage.h"
#include "MessageBase.h"

template <class Archive>
class LIDARRAYS_API OdometrySimulinkMessage : public MessageBase<Archive>
{
public:
	float X;
	float Y;
	float Z;
	float VX;
	float VY;
	float VZ;
	float Phi;

	void serialize(Archive & ar) override
	{
		ar(X, Y, Z);
		ar(VX, VY, VZ);
		ar(Phi);
	}

	std::string ToString() override
	{
		return std::string();
	}
};
