// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUtility.h"

DEFINE_LOG_CATEGORY(MyLog);

bool UMyUtility::GetStringFromTxtFile(const FString& FileName, FString& OutString)
{
	FString FilePath = FPaths::ProjectDir() + FileName;

	return FFileHelper::LoadFileToString(OutString, *FilePath);
}

void UMyUtility::WaitForSec(const float SecForWait)
{
	FPlatformProcess::Sleep(SecForWait);
}
