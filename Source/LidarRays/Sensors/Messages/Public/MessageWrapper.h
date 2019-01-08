#pragma once

#include <CoreMinimal.h>
#include "MessageBase.h"
#include <cereal/archives/binary.hpp>
#include <simulink_interface/archive.hpp>
#include "MessageWrapper.generated.h"

UCLASS(Blueprintable)
class LIDARRAYS_API UOutgoingMessage : public UObject
{
	GENERATED_BODY()

public:
	MessageBase<cereal::BinaryOutputArchive>* message = nullptr;
	~UOutgoingMessage()
	{
		if (message)
			delete message;
	}
};

UCLASS(Blueprintable)
class LIDARRAYS_API UOutgoingSimulinkMessage : public UObject
{
	GENERATED_BODY()

public:
	MessageBase<simulink::SimulinkOutputArchive>* message = nullptr;
	~UOutgoingSimulinkMessage()
	{
		if (message)
			delete message;
	}
};

UCLASS(Blueprintable)
class LIDARRAYS_API UIncomingMessage : public UObject
{
	GENERATED_BODY()

public:
	MessageBase<cereal::BinaryInputArchive>* message = nullptr;

	~UIncomingMessage()
	{
		if (message)
			delete message;
	}
};

UCLASS(Blueprintable)
class LIDARRAYS_API UIncomingSimulinkMessage : public UObject
{
	GENERATED_BODY()

public:
	MessageBase<simulink::SimulinkInputArchive>* message = nullptr;
	~UIncomingSimulinkMessage()
	{
		if (message)
			delete message;
	}
};
