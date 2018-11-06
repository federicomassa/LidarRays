#include "TwistMessage.h"

FString UTwistMessage::ToString()
{
	FString converted;

	if (Linear.size() != 3 || Angular.size() != 3)
		return FString("\nERROR in UTwistMessage::ToString: Bad twist message\n");
	
	converted += FString(TEXT("Timestamp: %f\n"), Timestamp);
	
	converted += FString(TEXT("Linear : {\n"));
	converted += FString::Printf(TEXT("\tx: %f\n\ty: %f\n\tz: %f\n"), Linear[0], Linear[1], Linear[2]);

	converted += FString(TEXT("Angular : {\n"));
	converted += FString::Printf(TEXT("\tx: %f\n\ty: %f\n\tz: %f\n"), Angular[0], Angular[1], Angular[2]);

	return converted;
}
