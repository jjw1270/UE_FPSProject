// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TCPSubsystem.generated.h"

/**
 * 
 */

UENUM()
enum class EPacketType : uint16
{
	None													= 0,

	S2C_Ping												= 1,
	C2S_Ping												= 2,

	S2C_CastMessage											= 10,

	S2C_ConnectSuccess										= 100,

//----------------------------Login----------------------------------------------------
	C2S_ReqSignIn											= 1000,
	S2C_ResSignIn_Success									= 1001,
	S2C_ResSignIn_Fail_InValidID							= 1002,
	S2C_ResSignIn_Fail_InValidPassword						= 1003,
	S2C_ResSignIn_Fail_AlreadySignIn						= 1004,

	C2S_ReqSignUp											= 1010,
	S2C_ResSignUp_Success									= 1011,
	S2C_ResSignUp_Fail_ID									= 1012,
	S2C_ResSignUp_Fail_NickName								= 1012,
	S2C_ResSignUp_Fail_ID_NickName							= 1013,

//-------------------------------------------------------------------------------------

	C2S_ReqMatchMaking = 1100,
	S2C_ResMatchMaking_DediIP = 1101,

	C2S_ReqCancelMatchMaking = 1110,

	Max,
};

struct FPacketData
{
public:
	FPacketData() : PacketType(EPacketType::None), Payload() {}
	FPacketData(EPacketType NewPacketType) : PacketType(NewPacketType), Payload() {}
	FPacketData(EPacketType NewPacketType, FString Payload) : PacketType(NewPacketType), Payload(Payload) {}
	FPacketData(uint16_t NewPacketTypeInt, FString Payload) : PacketType(static_cast<EPacketType>(NewPacketTypeInt)), Payload(Payload) {}

	EPacketType PacketType;
	FString Payload;
};

//packetType, payload
DECLARE_MULTICAST_DELEGATE_TwoParams(FDele_RecvPacket, const EPacketType&, const FString&);

const int32 HeaderSize{ 4 };

UCLASS()
class FPS_PROJECT_API UTCPSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

protected:
	bool GetIPAndPortFromTxtFile(FString& OutIP, int32& OutPort, const FString& FileName);

protected:
	void PrintSocketError(const FString& Text);

	class FSocket* Connect(const FString& IP, const int32& PortNum);

	bool Send(class FSocket*& TargetSocket, const FPacketData& SendPacket);

public:
	bool Recv(FSocket*& TargetSocket, FPacketData& OutRecvPacket);

	void DestroySocket(class FSocket*& TargetSocket);

/* < Funcs You Can Use on other class > ---------------------*/
public:
	bool SendToLoginServer(const FPacketData& SendPacket);
/*-----------------------------------------------------------*/

protected:
	FPacketData RecvPacketData;

	FTimerHandle ManageRecvPacketHandle;

public:
	FORCEINLINE void SetRecvPacket(const FPacketData& RecvPacket) { RecvPacketData = RecvPacket; };

protected:
	UFUNCTION()
	// Your Custom Codes In this Func
	void ManageRecvPacket();

public:
	// MultiCast Delegates You Can Use
	FDele_RecvPacket RecvPacketDelegate;

};

class FPS_PROJECT_API FClientThread : public FRunnable
{
public:
	FClientThread(class UTCPSubsystem* NewTCPSubsystem, class FSocket* TargetSocket);

	~FClientThread() override;

protected:
	bool Init() override;

	uint32 Run() override;

	void Exit() override;
	
	//void Stop() override;

private:
	class UTCPSubsystem* TCPSubsystem;

	class FSocket* ClientSocket;

	FRunnableThread* Thread;
};
