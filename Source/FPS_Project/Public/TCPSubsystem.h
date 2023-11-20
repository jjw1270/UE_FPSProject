// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TCPSubsystem.generated.h"

/**
 * 
 */

enum class EPacketType : uint16
{
	None													= 0,

	S2C_Ping												= 1,
	C2S_Ping												= 2,

	S2C_CastMessage											= 10,

	S2C_ConnectSuccess										= 100,			//not use in web server

//----------------------------Login----------------------------------------------------
	C2S_ReqSignIn											= 1000,
	S2C_ResSignIn_Success									= 1001,
	S2C_ResSignIn_Fail_InValidID							= 1002,
	S2C_ResSignIn_Fail_InValidPassword						= 1003,
	S2C_ResSignIn_Fail_AlreadySignIn						= 1004,

	C2S_ReqSignUpIDPwd										= 1010,
	S2C_ResSignUpIDPwd_Success								= 1011,
	S2C_ResSignUpIDPwd_Fail_ExistID							= 1012,

	C2S_ReqSignUpNickName									= 1020,
	S2C_ResSignUpNickName_Success							= 1021,
	S2C_ResSignUpNickName_Fail_ExistNickName				= 1022,
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

DECLARE_MULTICAST_DELEGATE_ThreeParams(FDele_RecvPacket, const FString&, const int32&, bool);

const int32 HeaderSize{ 4 };

UCLASS()
class FPS_PROJECT_API UTCPSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	class FClientThread* ClientThread;

	FPacketData RecvPacketData;
	FTimerHandle ManageRecvPacketHandle;

protected:
	class FSocket* Connect(const int32& PortNum, const FString& IP);

public:
	bool Recv(FPacketData& OutRecvPacket);

	FORCEINLINE void SetRecvPacket(const FPacketData& RecvPacket) { RecvPacketData = RecvPacket; };

	/* < Funcs You Can Use on other class > -----------------*/
public:
	void ConnectToTCPServer(const int32& PortNum, const FString& IP);

	void DestroySocket();

	bool Send(const FPacketData& SendPacket);

	bool SendToLoginServer(const FPacketData& SendPacket);

	bool IsConnect();
	/*-----------------------------------------------------*/

	/* < MultiCast Delegates You Can Use > ----------------*/
public:
	FDele_RecvPacket RecvPacketDelegate;
	/*-----------------------------------------------------*/

protected:
	// For Socket Error Log
	void PrintSocketError(const FString& Text);

protected:
	/* Your Custom Codes In this Func----------------------*/
	UFUNCTION()
	void ManageRecvPacket();
	/*-----------------------------------------------------*/

	/* Your Custom Vars In Here----------------------------*/
protected:

	/*-----------------------------------------------------*/
};

class FPS_PROJECT_API FClientThread : public FRunnable
{
public:
	FClientThread(class UTCPSubsystem* NewTCPSubsystem);

	virtual ~FClientThread();

protected:
	virtual bool Init() override;

	virtual uint32 Run() override;

	virtual void Stop() override;

	virtual void Exit() override;

private:
	class UTCPSubsystem* TCPSubsystem;

private:
	bool bStopThread;

};
