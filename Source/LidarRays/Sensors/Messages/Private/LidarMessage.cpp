#include "LidarMessage.h"

void FLidarMessage::Empty()
{
	Points.clear();
}

FLidarMessage::FLidarMessage()
{
	Empty();
}
