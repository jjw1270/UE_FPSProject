// Fill out your copyright notice in the Description page of Project Settings.


#include "Login/LoginGameMode.h"
#include "FPS_Project.h"
#include "TCPSubsystem.h"

void ALoginGameMode::StartPlay()
{
	Super::StartPlay();

	UTCPSubsystem* TCPSubsystem = GetGameInstance()->GetSubsystem<UTCPSubsystem>();
	CHECK_VALID(TCPSubsystem);

	FPacketData SendPacket(EPacketType::C2S_Ping, TEXT("안녕!"));
	TCPSubsystem->SendToLoginServer(SendPacket);
}
