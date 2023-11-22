// Fill out your copyright notice in the Description page of Project Settings.


#include "Login/LoginPlayerController.h"
#include "FPS_Project.h"
#include "Blueprint/UserWidget.h"

void ALoginPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CHECK_VALID(StartScreenWidgetClass);
	StartScreenWidget = CreateWidget(this, StartScreenWidgetClass);
	StartScreenWidget->AddToViewport();

	SetShowMouseCursor(true);
	SetInputMode(FInputModeUIOnly());
}
