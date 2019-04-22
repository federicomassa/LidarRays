#pragma once

#include <array>
#include "MessageBase.h"
#include "PoseMessage.generated.h"

//UCLASS(Blueprintable, ClassGroup = (Messages))
USTRUCT(BlueprintType)
struct FPoseMessage : public FMessageBase
{
	GENERATED_BODY()

	float x, y, theta;

public:
	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(x, y, theta);
	}

	std::string ToString() override
	{
		return "PoseMessage-NOT-IMPLEMENTED";
	}
};