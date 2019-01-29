#pragma once

#include <Array.h>
#include "ArrayWrapper.generated.h"

UCLASS()
class UArrayWrapper : public UObject
{
	GENERATED_BODY()

	TArray<uint8> Data;
public:
	void SetData(TArray<uint8> data)
	{
		this->Data = data;
	}

	void Add(uint8 i)
	{
		Data.Add(i);
	}

	UFUNCTION(BlueprintCallable)
		TArray<uint8> GetData() const { return Data; }
};
