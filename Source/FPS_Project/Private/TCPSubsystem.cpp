// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPSubsystem.h"

#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "IPAddress.h"

// Custom Includes
#include "FPS_project.h"
#include "MyUtility.h"

bool UTCPSubsystem::GetIPAndPortFromTxtFile(FString& OutIP, int32& OutPort, const FString& FileName)
{
	FString IPPortConfig;
	if (!UMyUtility::GetStringFromTxtFile(FileName, IPPortConfig))
	{
		ABLOG(Warning, TEXT("Get String From Txt File Fail"));
		return false;
	}

	// Parsing Data
	// IP:Port

	FString TempIP, TempPort;
	IPPortConfig.Split(":", &TempIP, &TempPort);

	OutIP = TempIP;
	OutPort = FCString::Atoi(*TempPort);

	return true;
}

void UTCPSubsystem::PrintSocketError(const FString& Text)
{
	ESocketErrors SocketErrorCode = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLastErrorCode();
	const TCHAR* SocketError = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetSocketError(SocketErrorCode);

	ABLOG(Error, TEXT("[TCP] %s Error : %s"), *Text, SocketError);
}

FSocket* UTCPSubsystem::Connect(const FString& IP, const int32& PortNum)
{
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (!SocketSubsystem)
	{
		return nullptr;
	}

	FSocket* ClientSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("ClientSocket"), false);
	if (!ClientSocket)
	{
		ABLOG(Error, TEXT("Create Socket Failure"));
		return nullptr;
	}

	FAddressInfoResult AddressInfoResult = SocketSubsystem->GetAddressInfo(*IP, *FString::FromInt(PortNum), EAddressInfoFlags::CanonicalName, NAME_Stream);
	
	if (ClientSocket->Connect(*AddressInfoResult.Results[0].Address))
	{
		return ClientSocket;
	}
	else
	{
		DestroySocket(ClientSocket);
	}

	return nullptr;
}

bool UTCPSubsystem::Send(FSocket*& TargetSocket, const FPacketData& SendPacket)
{
	uint8_t* PayloadBuffer = nullptr;
	uint16_t PayloadSize = 0;

	if (!SendPacket.Payload.IsEmpty())
	{
		// FString to UTF8 const char* type buffer
		ANSICHAR* PayloadCharBuf = TCHAR_TO_UTF8(*SendPacket.Payload);
		PayloadSize = strlen(PayloadCharBuf);
		PayloadBuffer = reinterpret_cast<uint8_t*>(PayloadCharBuf);
	}

	// Send Header
	ABLOG(Warning, TEXT("Payload Size : %d"), (int32)PayloadSize);

	const uint16_t Type = static_cast<uint16_t>(SendPacket.PacketType);

	uint8_t HeaderBuffer[HeaderSize] = { 0, };

	FMemory::Memcpy(&HeaderBuffer, &PayloadSize, 2);
	FMemory::Memcpy(&HeaderBuffer[2], &Type, 2);

	int32 BytesSent = 0;
	bool bSendBuffer = TargetSocket->Send(HeaderBuffer, HeaderSize, BytesSent);
	if (!bSendBuffer)
	{
		PrintSocketError(TEXT("Send"));
		return false;
	}

	// Send Payload
	if (PayloadBuffer != nullptr)
	{
		BytesSent = 0;
		bSendBuffer = TargetSocket->Send(PayloadBuffer, PayloadSize, BytesSent);
		if (!bSendBuffer)
		{
			PrintSocketError(TEXT("Send"));
			return false;
		}
	}

	return true;
}

bool UTCPSubsystem::Recv(FSocket*& TargetSocket, FPacketData& OutRecvPacket)
{
	if (TargetSocket->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromSeconds(2.f)))
	{
		TArray<uint8_t> HeaderBuffer;
		HeaderBuffer.AddZeroed(HeaderSize);

		// Recv Header
		int BytesRead = 0;
		bool bRecvHeader = TargetSocket->Recv(HeaderBuffer.GetData(), HeaderSize, BytesRead);
		if (!bRecvHeader)
		{
			PrintSocketError(TEXT("Receive Header"));
			return false;
		}

		uint16 RecvPayloadSize;
		uint16 RecvPacketType;

		// Get Size and Type from HeaderBuffer
		FMemory::Memcpy(&RecvPayloadSize, HeaderBuffer.GetData(), sizeof(uint16_t));
		FMemory::Memcpy(&RecvPacketType, HeaderBuffer.GetData() + sizeof(uint16_t), sizeof(uint16_t));

		OutRecvPacket.PacketType = static_cast<EPacketType>(RecvPacketType);
		ABLOG(Warning, TEXT(" [Recv] PacketType : %d, PayloadSize : %d"), RecvPacketType, RecvPayloadSize);

		// Recv Payload
		if (RecvPayloadSize > 0)
		{
			uint8_t* PayloadBuffer = new uint8_t[RecvPayloadSize + 1];

			BytesRead = 0;
			bool bRecvPayload = TargetSocket->Recv(PayloadBuffer, RecvPayloadSize, BytesRead);

			if (!bRecvPayload)
			{
				PrintSocketError(TEXT("Receive Payload"));
				return false;
			}
			PayloadBuffer[RecvPayloadSize] = '\0';

			//Utf8 to FStirng
			FString PayloadString;
			PayloadString = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(PayloadBuffer)));

			ABLOG(Warning, TEXT(" [Recv] Payload : %s"), *PayloadString);
			OutRecvPacket.Payload = PayloadString;

			delete[] PayloadBuffer;
			PayloadBuffer = nullptr;
		}

	}
	return true;
}

void UTCPSubsystem::DestroySocket(FSocket*& TargetSocket)
{
	// Clean Socket
	if (TargetSocket)
	{
		if (TargetSocket->GetConnectionState() == SCS_Connected)
		{
			TargetSocket->Close();
			ABLOG(Warning, TEXT("Close Socket"));
		}

		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(TargetSocket);
		ABLOG(Warning, TEXT("Destroy Socket"));

		TargetSocket = nullptr;
		delete TargetSocket;
	}
}

bool UTCPSubsystem::SendToLoginServer(const FPacketData& SendPacket)
{
	FString IP;
	int32 Port;
	if (!GetIPAndPortFromTxtFile(IP, Port, TEXT("LoginServerConfig.txt")))
	{
		return false;
	}

	// connect
	FSocket* ClientSocket = Connect(IP, Port);
	if (!ClientSocket)
	{
		PrintSocketError("Connect");
		return false;
	}

	// send
	if (!Send(ClientSocket, SendPacket))
	{
		PrintSocketError("Send");
		return false;
	}

	// make thread
	new FClientThread(this, ClientSocket);

	return true;
}

void UTCPSubsystem::ManageRecvPacket()
{
	if (RecvPacketData.PacketType == EPacketType::None)
	{
		return;
	}

	if (RecvPacketDelegate.IsBound())
	{
		int32 PacketCode = static_cast<int32>(RecvPacketData.PacketType);

		/*switch (RecvPacketData.PacketType)
		{

		default:
			break;
		}*/
	}

	RecvPacketData = FPacketData();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//ClientThread Class

FClientThread::FClientThread(UTCPSubsystem* NewTCPSubsystem, class FSocket* TargetSocket)
	: TCPSubsystem(NewTCPSubsystem), ClientSocket(TargetSocket)
{
	Thread = FRunnableThread::Create(this, TEXT("ClientThread"));
}

FClientThread::~FClientThread()
{
	if (Thread)
	{
		Thread->WaitForCompletion();
		Thread->Kill();
		delete Thread;
		Thread = nullptr;
	}

	delete TCPSubsystem;
	TCPSubsystem = nullptr;

	delete ClientSocket;
	ClientSocket = nullptr;
}

bool FClientThread::Init()
{
	return true;

	ABLOG(Warning, TEXT("Start Thread"));
}

uint32 FClientThread::Run()
{
	FPacketData RecvPacketData;
	bool RecvByte = TCPSubsystem->Recv(ClientSocket, RecvPacketData);
	if (!RecvByte)
	{
		ABLOG(Error, TEXT("Recv Error, Stop Thread"));
	}
	else if (RecvPacketData.PacketType != EPacketType::None)
	{
		TCPSubsystem->SetRecvPacket(RecvPacketData);
	}

	return 0;
}

void FClientThread::Exit()
{
	TCPSubsystem->DestroySocket(ClientSocket);
	ABLOG(Warning, TEXT("End Thread"));
}
