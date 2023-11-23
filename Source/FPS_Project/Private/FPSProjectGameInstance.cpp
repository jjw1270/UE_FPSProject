// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSProjectGameInstance.h"
#include "FPS_Project.h"
#include "TCPSubsystem.h"

void UFPSProjectGameInstance::Shutdown()
{
	UTCPSubsystem* TCPSubsystem = GetSubsystem<UTCPSubsystem>();
	if (TCPSubsystem)
	{
		TCPSubsystem->SendToLoginServer(FPacketData(EPacketType::C2S_ReqSignOut, UserID));
	}

	Super::Shutdown();
}
