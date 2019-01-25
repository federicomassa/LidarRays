#pragma once
#include <string>
#include <CoreMinimal.h>
#include "MessageBase.generated.h"

UCLASS(Blueprintable, ClassGroup = (Messages))
class UMessageBase : public UObject
{
	GENERATED_BODY()

public:
	virtual std::string ToString();
	virtual ~UMessageBase() {}
};