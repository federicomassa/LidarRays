// =========== Test Message ============= //

#pragma once
#include "MessageBase.h"
#include <string>
#include <sstream>

template <class Archive>
class TestMessage : public MessageBase<Archive>
{
public:
	int n = 3;
	float f = -4.5;

	void serialize(Archive& ar) override
	{
		ar(n);
		ar(f);
	}
	std::string ToString() override
	{
		std::stringstream ss;

		ss << "TestMessage: n = ";
		ss << n;
		ss << " f = ";
		ss << f;
		ss << "\n";

		return ss.str();
	}

};