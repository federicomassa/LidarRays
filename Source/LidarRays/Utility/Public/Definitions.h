#pragma once

#include <CoreMinimal.h>

UENUM(BlueprintType)
enum class EVehicleModelEnum : uint8
{
	VM_PhysX UMETA(DisplayName = "PhysX"),
	VM_Unicycle UMETA(DisplayName = "Unicycle"),
	VM_SingleTrack UMETA(DisplayName = "SingleTrack")
};