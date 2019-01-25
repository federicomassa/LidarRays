#include "LidarMessage.h"

void ULidarMessage::Empty()
{
	Points.clear();
}

ULidarMessage::ULidarMessage()
{
	Empty();
}
