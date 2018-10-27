#include "LidarMessage.h"

void ULidarMessage::Empty()
{
	timestamp = 0;
	PointsX.clear();
	PointsY.clear();
	PointsZ.clear();
}

ULidarMessage::ULidarMessage()
{
	Empty();
}
