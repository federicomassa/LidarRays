#include "TwistStampedMessage.h"

FString UTwistStampedMessage::ToString()
{
	FString converted;
	converted += FString(TEXT("Timestamp: %f\n"), Timestamp);
	converted += Twist->ToString();

	return converted;
}
