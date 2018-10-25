// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AnyCustomData.generated.h"

USTRUCT(BlueprintType)
struct FAnyCustomData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy Color") 
		FString Name = "Victory!";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy Color") 
		int32 Count = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy Color") 
		float Scale = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Joy Color") 
		FLinearColor Color = FLinearColor::Red;

	FAnyCustomData() {}
};

FORCEINLINE FArchive& operator<<(FArchive &Ar, FAnyCustomData& TheStruct) {
	Ar << TheStruct.Name; Ar << TheStruct.Count; Ar << TheStruct.Scale; Ar << TheStruct.Color;

	return Ar;
}