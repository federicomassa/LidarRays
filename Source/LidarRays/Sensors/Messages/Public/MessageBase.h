#pragma once
#include <string>
#include <CoreMinimal.h>
#include "MessageBase.generated.h"

//UCLASS(Blueprintable, ClassGroup = (Messages))
USTRUCT(BlueprintType)
struct FMessageBase
{
	GENERATED_BODY()

	virtual std::string ToString();
	virtual ~FMessageBase() {}
};