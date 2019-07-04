#include "GPSSensor.h"
#include <RunnableThread.h>
#include "UDPSender.h"
#include "OdometryMessage.h"
#include "MessageSerializerComponent.h"
#include <fstream>

FGPSSensor::FGPSSensor(const ATazioVehicle* Owner, AUDPSender* Sender)
{
	check(Owner);
	_Owner = Owner;

	check(Sender);
	_Sender = Sender;

	Sender->Start("GPS Sensor", "192.168.105.3", 8892, true);
	Thread = FRunnableThread::Create(this, TEXT("GPS Sensor"), 0, TPri_Normal); //windows default = 8mb for thread, could specify more
} 

FGPSSensor::~FGPSSensor()
{
	Stop();
	Thread->WaitForCompletion();

	delete Thread;
	Thread = nullptr;
}

bool FGPSSensor::Init()
{
	return true;
}

uint32 FGPSSensor::Run()
{
	auto InitTime = std::chrono::steady_clock::now();

	// TEMP
	//std::ofstream timestamps("D:/gps_timestamps.txt");

	auto PreIterTime = std::chrono::time_point<std::chrono::steady_clock>();
	auto PostIterTime = std::chrono::time_point<std::chrono::steady_clock>();

	FOdometryMessage GPSMessage;

	while (!_KillRequested)
	{
		// Pre iteration time 
		PreIterTime = std::chrono::steady_clock::now();

		// All work...
		auto state = _Owner->GetLatestState();
		if (state == nullptr)
			continue;

		GPSMessage.x = state->x;
		GPSMessage.y = state->y;
		GPSMessage.z = 0.0;

		GPSMessage.pitch = 0.0;
		GPSMessage.roll = 0.0;
		GPSMessage.yaw = state->theta;

		auto Data = UMessageSerializerComponent::SerializeOdometryMessage(GPSMessage, true);
		_Sender->SendData(Data);


		//timestamps << std::chrono::duration_cast<std::chrono::nanoseconds>(PreIterTime - InitTime).count()*1E-9 << '\n';

		// Time after job
		PostIterTime = std::chrono::steady_clock::now();
		// Sleep
		double IterationTime = std::chrono::duration_cast<std::chrono::nanoseconds>(PostIterTime - PreIterTime).count()*1E-9;
	
		if (0.004 - IterationTime > 0)
			FPlatformProcess::Sleep(0.004 - IterationTime);
	}

	return 0;
}

void FGPSSensor::Stop()
{
	_KillRequested = true;
}