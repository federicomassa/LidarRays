#include "LidarMessage.h"

ULidarMessage::Point::Point(float x, float y, float z)
{
	X = x;
	Y = y;
	Z = z;
}


void ULidarMessage::Empty()
{
	timestamp = 0;
	Data.clear();
}

ULidarMessage::ULidarMessage()
{
	Empty();
}
