// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FPSProjectGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FPS_PROJECT_API UFPSProjectGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
protected:
	virtual void Shutdown() override;

public:
	FORCEINLINE void SetUserID(const FString& NewUserID) { UserID = NewUserID; }
	FORCEINLINE const FString& GetUserID() const { return UserID; }

protected:
	FString UserID;
};
