// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuUI.generated.h"

/**
 * 
 */
UCLASS()
class LIDARRAYS_API UMenuUI : public UUserWidget
{
	GENERATED_BODY()

	/*
	 * Opens a file dialog for the specified data. Leave FileTypes empty to be able to select any files.
	 * Filetypes must be in the format of: <File type Description>|*.<actual extension>
	 * You can combine multiple extensions by placing ";" between them
	 * For example: Text Files|*.txt|Excel files|*.csv|Image Files|*.png;*.jpg;*.bmp will display 3 lines for 3 different type of files.
	 */
	UFUNCTION(BlueprintCallable, Category = "File Dialog")
	void OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileNames);
};
