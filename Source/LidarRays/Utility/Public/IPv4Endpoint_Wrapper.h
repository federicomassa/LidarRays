#pragma once

#include <IPv4Endpoint.h>
#include <Containers/Map.h>
#include "IPv4Endpoint_Wrapper.generated.h"

USTRUCT(Blueprintable)
struct FIPv4Endpoint_Wrapper
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString Address;

	UPROPERTY(EditAnywhere)
	uint32 Port;
};