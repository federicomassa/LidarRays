#pragma once

#include <array>
#include "MessageBase.h"
#include "RaceControlMessage.generated.h"

#pragma once

//UCLASS(Blueprintable, ClassGroup = (Messages))
USTRUCT(BlueprintType)
struct FRaceControlMessage : public FMessageBase
{
	GENERATED_BODY()


	uint8 race_mode;
	uint8 overtaking_status;
	double speed_limit;
	double power_limit;
	uint8 race_position;
	uint8 lap_remaining;
	double timestamp;

	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(race_mode);
		ar(overtaking_status);
		ar(speed_limit);
		ar(power_limit);
		ar(race_position);
		ar(lap_remaining);
		ar(timestamp);
	}

	std::string ToString() override
	{
		return std::string();
	}
};