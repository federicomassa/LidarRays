// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "UDPSender.h"
#include "MyPawn.generated.h"

UCLASS()
class LIDARRAYS_API AMyPawn : public ADefaultPawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = UDP)
	AUDPSender* UDPSender;
public:
	// Sets default values for this pawn's properties
	AMyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = UDP)
	AUDPSender* GetUDPSender();
	
	
};
