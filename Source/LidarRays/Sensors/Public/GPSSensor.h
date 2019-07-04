#pragma once

#include <Runnable.h>
#include <chrono>
#include "TazioVehicle.h"

class AUDPSender;

class FGPSSensor : public FRunnable
{
	const ATazioVehicle* _Owner = nullptr;
	class FRunnableThread* Thread = nullptr;

	AUDPSender* _Sender = nullptr;

	// When this is set to true, run finishes current iteration and exits
	bool _KillRequested = false;
public:
	FGPSSensor(const ATazioVehicle*, AUDPSender*);
	~FGPSSensor();
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
};