#pragma once

#ifndef __PACKET_H__ 
#define __PACKET_H__

enum class EPacket
{
	None													= 0,

	S2C_Ping												= 1,
	C2S_Ping												= 2,

	S2C_CastMessage											= 10,

	S2C_ConnectSuccess										= 100,

	///////////////////////////////////////////////////////////////////////////

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
	
	C2S_ReqMatchMaking										= 1100,
	S2C_ResMatchMaking_DediIP								= 1101,

	C2S_ReqCancelMatchMaking								= 1110,

	///////////////////////////////////////////////////////////////////////////

	C2S_ReqDediTCPConnect									= 3000,

	S2C_ReqDediServer										= 3100,
	C2S_ResDediServer										= 3101,

	///////////////////////////////////////////////////////////////////////////

	Max,
};

#pragma pack(1)
#pragma pack()

#endif