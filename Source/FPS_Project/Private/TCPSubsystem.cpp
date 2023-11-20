// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPSubsystem.h"

#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "IPAddress.h"
// Custom Includes
#include "FPS_project.h"

FSocket* UTCPSubsystem::Connect(const int32& PortNum, const FString& IP)
{
	FSocket* ClientSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("ClientSocket"), false);
	if (!ClientSocket)
	{
		ABLOG(Error, TEXT("Create Socket Failure"));
		return nullptr;
	}

	FIPv4Address IPv4Address;
	FIPv4Address::Parse(IP, IPv4Address);

	TSharedPtr<FInternetAddr> SocketAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	SocketAddress->SetPort(PortNum);
	SocketAddress->SetIp(IPv4Address.Value);

	if (ClientSocket->Connect(*SocketAddress))
	{
		return ClientSocket;
	}
	else
	{
		DestroySocket();
	}
	
	return nullptr;
}

bool UTCPSubsystem::Recv(FPacketData& OutRecvPacket)
{
	return false;
}

void UTCPSubsystem::ConnectToTCPServer(const int32& PortNum, const FString& IP)
{
}

void UTCPSubsystem::DestroySocket()
{
}

bool UTCPSubsystem::Send(const FPacketData& SendPacket)
{
	return false;
}

bool UTCPSubsystem::SendToLoginServer(const FPacketData& SendPacket)
{
	// connect
	// get ip, port from LoginServerConfig.txt
	//FSocket* ClientSocket = 

	// send
	// make thread

	return false;
}

bool UTCPSubsystem::IsConnect()
{
	return false;
}

void UTCPSubsystem::PrintSocketError(const FString& Text)
{
}

void UTCPSubsystem::ManageRecvPacket()
{
}

FClientThread::FClientThread(UTCPSubsystem* NewTCPSubsystem)
{
}

FClientThread::~FClientThread()
{
}

bool FClientThread::Init()
{
	return false;
}

uint32 FClientThread::Run()
{
	return uint32();
}

void FClientThread::Stop()
{
}

void FClientThread::Exit()
{
}
