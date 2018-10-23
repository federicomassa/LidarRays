// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Runnable.h>

class APlayerController;
class AActor;
class UWorld;

//~~~~~ Multi Threading ~~~
class FSensorWorker : public FRunnable
{
	/** Singleton instance, can access the thread any time via static accessor, if it is active! */
	static  FSensorWorker* Runnable;
	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread;
	/** The Data Ptr */
	TArray<FVector>* LidarScan;
	/** The PC */
	APlayerController* ThePC;
	/** Stop this thread? Uses Thread Safe Counter */
	FThreadSafeCounter StopTaskCounter;
	// Next ray-cast
	FVector FindNextLidarPoint();

	// Pointer to car
	const AActor* Car = nullptr;

	// Pointer to world
	UWorld* World = nullptr;

private:
	int32 LidarPointsProcessed;
public:
	int32 TotalLidarPointsToProcess;
	//Done?
	bool IsFinished() const
	{
		return LidarPointsProcessed == TotalLidarPointsToProcess;
	}

	//~~~ Thread Core Functions ~~~
	//Constructor / Destructor
	FSensorWorker(TArray<FVector>& Data, const int32 IN_LidarPointsToProcess, APlayerController* IN_PC, const AActor* _Car, UWorld* _World );
	virtual ~FSensorWorker();

	// Begin FRunnable interface.
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	// End FRunnable interface

	/** Makes sure this thread has stopped properly */
	void EnsureCompletion();

	//~~~ Starting and Stopping Thread ~~~

	/*  Start the thread and the worker from static (easy access)!
		This code ensures only 1 sensor thread will be able to run at a time.
		This function returns a handle to the newly started instance.
	*/
	static FSensorWorker* JoyInit(TArray<FVector>& Data, const int32 IN_LidarPointsToProcess, APlayerController* IN_PC, const AActor* _Car, UWorld* _World);

	/** Shuts down the thread. Static so it can easily be called from outside the thread context */
	static void Shutdown();
	static bool IsThreadFinished();
};