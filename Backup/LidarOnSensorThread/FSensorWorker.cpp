// Fill out your copyright notice in the Description page of Project Settings.

#include "FSensorWorker.h"
#include <GameFramework/PlayerController.h>
#include <RunnableThread.h>
#include <GameFramework/Actor.h>
#include <Engine/World.h>

FSensorWorker* FSensorWorker::Runnable = nullptr;

FVector FSensorWorker::FindNextLidarPoint()
{
	FVector CurrentLocation = Car->GetActorLocation();
	FRotator ThisRotation = Car->GetActorRotation() + FRotator(0, LidarPointsProcessed*0.1, 0);
	FVector EndLocation = CurrentLocation + 1000.f * (ThisRotation.Vector());

	FHitResult Hit;

	World->LineTraceSingleByChannel(
		Hit,
		CurrentLocation,
		EndLocation,
		ECollisionChannel::ECC_Visibility
	);

	return Hit.Location - CurrentLocation;
}

FSensorWorker::FSensorWorker(TArray<FVector>& Data, const int32 IN_LidarPointsToProcess, APlayerController * IN_PC, const AActor* _Car, UWorld* _World) :
	ThePC(IN_PC),
	TotalLidarPointsToProcess(IN_LidarPointsToProcess),
	StopTaskCounter(0),
	LidarPointsProcessed(0),
	Car(_Car),
	World(_World)
{
	LidarScan = &Data;
	
	// Create thread
	FString ThreadName(FString::Printf(TEXT("MyThreadName%i"), WorkerCounter.Increment()));
	Thread = FRunnableThread::Create(this, TEXT("UniqueThreadName"), 0, TPri_BelowNormal);

	/*if (!Car)
	{
		UE_LOG(LogTemp, Error, TEXT("Sensor worker needs a valid Actor pointer"));
		return;
	}

	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("Sensor worker needs a valid World pointer"));
		return;
	}*/

}

FSensorWorker::~FSensorWorker()
{
	delete Thread;
	Thread = nullptr;
}

bool FSensorWorker::Init()
{
	if (ThePC)
	{
		ThePC->ClientMessage("*************************");
		ThePC->ClientMessage("  Sensor thread started!");
		ThePC->ClientMessage("*************************");
		return true;
	}

	return false;
}

uint32 FSensorWorker::Run()
{
	FPlatformProcess::Sleep(0.03);
	
	while (StopTaskCounter.GetValue() == 0 && !IsFinished())
	{
		LidarScan->Add(FindNextLidarPoint());
		LidarPointsProcessed++;
	}

	return 0;
}

void FSensorWorker::Stop()
{
	StopTaskCounter.Increment();
}

void FSensorWorker::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
}

FSensorWorker * FSensorWorker::JoyInit(TArray<FVector>& Data, const int32 IN_LidarPointsToProcess, APlayerController * IN_PC, const AActor* _Car, UWorld* _World)
{
	// Create new instance of thread if it doesn't exist
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new FSensorWorker(Data, IN_LidarPointsToProcess, IN_PC, _Car, _World);
	}

	return Runnable;
}

void FSensorWorker::Shutdown()
{
	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = nullptr;
	}
}

bool FSensorWorker::IsThreadFinished()
{
	if (Runnable)
		return Runnable->IsFinished();

	return true;
}






