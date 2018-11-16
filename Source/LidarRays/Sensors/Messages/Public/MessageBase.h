#pragma once
#include <string>

template <class Archive>
class MessageBase
{
public:
	virtual void serialize(Archive&) = 0;
	virtual std::string ToString() = 0;
	virtual ~MessageBase() {}
};