// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyUtility.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UMyUtility : public UObject
{
	GENERATED_BODY()
	
public:
	static bool GetStringFromTxtFile(const FString& FileName, FString& OutString);
	
};
