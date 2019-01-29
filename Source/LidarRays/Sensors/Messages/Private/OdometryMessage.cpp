#include "OdometryMessage.h"

FOdometryMessage::FOdometryMessage()
{
	//UE_LOG(LogTemp, Warning, TEXT("BUILDING GPS MESSAGE"));
	for (int i = 0; i < pose_covariance.size(); i++)
	{
		pose_covariance[i] = -1.f;
	}

	for (int i = 0; i < twist_covariance.size(); i++)
	{
		twist_covariance[i] = -1.f;
	}
}

FOdometryMessage::~FOdometryMessage()
{
	//UE_LOG(LogTemp, Warning, TEXT("!!!DESTROYING GPS MESSAGE!!!"));
}