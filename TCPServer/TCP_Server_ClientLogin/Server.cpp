#include <iostream>
#include <string>
#include <list>
#include <process.h>

using namespace std;

//--mysql
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

//socket
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32")

#include "MyUtility.h"
#include "Packet.h"
#include "PacketMaker.h"

sql::Connection* Sql_Connection;

const int HeaderSize = 4;

void ProcessPacket(SOCKET& ClientSocket, const EPacket& PacketType, char*& Payload)
{
	bool bSendSuccess = false;
	sql::PreparedStatement* Sql_PreStatement = nullptr;
	sql::ResultSet* Sql_Result = nullptr;

	switch (PacketType)
	{
	case EPacket::C2S_ReqSignUp:
	{
		//Parsing ID, Password, Username
		vector<string> ParsingData = MyUtility::ParsingString(Payload, ':');
		string ID = ParsingData[0];
		string Password = ParsingData[1];
		string UserName = ParsingData[2];

		//Check ID, Username is overlapped in DB userconfig
		string SqlQuery = "SELECT "
			"(SELECT COUNT(*) FROM userconfig WHERE ID = ?) AS ID_Exist, "
			"(SELECT COUNT(*) FROM userconfig WHERE NickName = ?) AS NickName_Exist";
		Sql_PreStatement = Sql_Connection->prepareStatement(SqlQuery);
		Sql_PreStatement->setString(1, ID);
		Sql_PreStatement->setString(2, MyUtility::MultibyteToUtf8(UserName));
		Sql_Result = Sql_PreStatement->executeQuery();

		if (Sql_Result->next())
		{
			if (Sql_Result->getInt("ID_Exist") > 0)
			{
				bSendSuccess = PacketMaker::SendPacket(&ClientSocket, EPacket::S2C_ResSignUp_Fail_ID);
				if (!bSendSuccess)
				{
					cout << "[" << (int)ClientSocket << "] Send Error : " << GetLastError() << endl;
				}
			}
			else if (Sql_Result->getInt("NickName_Exist") > 0)
			{
				bSendSuccess = PacketMaker::SendPacket(&ClientSocket, EPacket::S2C_ResSignUp_Fail_NickName);
				if (!bSendSuccess)
				{
					cout << "[" << (int)ClientSocket << "] Send Error : " << GetLastError() << endl;
				}
			}
			else
			{
				cout << "Create New Userconfig" << endl;
				// Create New Userconfig
				SqlQuery = "INSERT INTO userconfig(ID, NickName, Password) VALUES(?,?,?)";
				Sql_PreStatement = Sql_Connection->prepareStatement(SqlQuery);
				Sql_PreStatement->setString(1, ID);
				Sql_PreStatement->setString(2, MyUtility::MultibyteToUtf8(UserName));
				Sql_PreStatement->setString(3, Password);
				Sql_PreStatement->executeUpdate();

				bSendSuccess = PacketMaker::SendPacket(&ClientSocket, EPacket::S2C_ResSignUp_Success);
				if (!bSendSuccess)
				{
					cout << "[" << (int)ClientSocket << "] Send Error : " << GetLastError() << endl;
				}
			}
		}
	}
	break;
	case EPacket::C2S_ReqCheckIDUserName:
	{
		//Parsing ID, Username
		vector<string> ParsingData = MyUtility::ParsingString(Payload, ':');
		string ID = ParsingData[0];
		string Username = ParsingData[1];

		//Check ID and Username in DB
		string SqlQuery = "SELECT * FROM userconfig WHERE ID = ?";
		Sql_PreStatement = Sql_Connection->prepareStatement(SqlQuery);
		Sql_PreStatement->setString(1, ID);
		Sql_Result = Sql_PreStatement->executeQuery();

		if (Sql_Result->rowsCount() > 0)
		{
			//Check ID has Username
			if (Sql_Result->next())
			{
				string DB_NickName = Sql_Result->getString("NickName");
				if (DB_NickName == Username)
				{
					bSendSuccess = PacketMaker::SendPacket(&ClientSocket, EPacket::S2C_ResCheckIDUserName_Success);
					if (!bSendSuccess)
					{
						cout << "[" << (int)ClientSocket << "] Send Error : " << GetLastError() << endl;
					}
				}
				else
				{
					bSendSuccess = PacketMaker::SendPacket(&ClientSocket, EPacket::S2C_ResCheckIDUserName_Fail);
					if (!bSendSuccess)
					{
						cout << "[" << (int)ClientSocket << "] Send Error : " << GetLastError() << endl;
					}
				}
			}
		}
		else
		{
			//ID Not Exist in DB
			bSendSuccess = PacketMaker::SendPacket(&ClientSocket, EPacket::S2C_ResCheckIDUserName_Fail_IDNotExist);
			if (!bSendSuccess)
			{
				cout << "[" << (int)ClientSocket << "] Send Error : " << GetLastError() << endl;
			}
		}
	}
	break;
	case EPacket::C2S_ReqNewPassword:
	{
		//Parsing ID, Password
		vector<string> ParsingData = MyUtility::ParsingString(Payload, ':');
		string ID = ParsingData[0];
		string NewPassword = ParsingData[1];

		string SqlQuery = "UPDATE userconfig SET Password = ? WHERE ID = ?";
		Sql_PreStatement = Sql_Connection->prepareStatement(SqlQuery);
		Sql_PreStatement->setString(1, NewPassword);
		Sql_PreStatement->setString(2, ID);
		Sql_PreStatement->executeUpdate();

		bSendSuccess = PacketMaker::SendPacket(&ClientSocket, EPacket::C2S_ResNewPassword);
		if (!bSendSuccess)
		{
			cout << "[" << (int)ClientSocket << "] Send Error : " << GetLastError() << endl;
		}
	}
	break;
	default:
		break;
	}

	delete Sql_Result;
	delete Sql_PreStatement;
}

unsigned WINAPI ServerThread(void* arg)
{
	cout << "Thread Start" << endl;

	SOCKET ClientSocket = *(SOCKET*)arg;

	// Recv Header
	char HeaderBuffer[HeaderSize] = { 0, };
	int RecvByte = recv(ClientSocket, HeaderBuffer, HeaderSize, MSG_WAITALL);
	if (RecvByte == 0 || RecvByte < 0) //close, Error
	{
		cout << "[" << (int)ClientSocket << "] Recv Error : " << GetLastError() << endl;
		closesocket(ClientSocket);
		return 0;
	}

	unsigned short PayloadSize = 0;
	unsigned short PacketType = 0;

	memcpy(&PayloadSize, HeaderBuffer, 2);
	memcpy(&PacketType, &HeaderBuffer[2], 2);

	printf("[Receive] Payload size : %d, Packet type : %d\n", PayloadSize, PacketType);

	char* Payload = nullptr;

	// Recv Payload
	if (PayloadSize > 0)
	{
		Payload = new char[PayloadSize + 1];

		RecvByte = recv(ClientSocket, Payload, PayloadSize, MSG_WAITALL);
		if (RecvByte == 0 || RecvByte < 0)
		{
			cout << "[" << (int)ClientSocket << "] Recv Error : " << GetLastError() << endl;
			closesocket(ClientSocket);
			return 0;
		}
		Payload[PayloadSize] = '\0';
		cout << "Data : " << Payload << endl;
	}

	ProcessPacket(ClientSocket, static_cast<EPacket>(PacketType), Payload);

	delete[] Payload;
	Payload = nullptr;

	if (closesocket(ClientSocket) != SOCKET_ERROR)
	{
		cout << "Close Socket" << endl;
	}

	cout << "Thread End" << endl;
	return 0;
}

int main()
{
	cout << "Connecting to DB Server... ";

	string Server, Username, Password;
	if (!MyUtility::GetConfigFromFile(Server, Username, Password))
	{
		cout << "Fail" << endl;
		cout << "Get Config txt Error" << endl;
		system("pause");
		exit(-1);
	}

	sql::Driver* Sql_Driver;
	try
	{
		Sql_Driver = get_driver_instance();
		Sql_Connection = Sql_Driver->connect(Server, Username, Password);
		cout << "Success!" << endl;
	}
	catch (sql::SQLException e)
	{
		cout << "Fail" << endl;
		cout << "Could not connect to data base : " << e.what() << endl;
		system("pause");
		exit(-1);
	}

	Sql_Connection->setSchema("FPSproject");

	cout << "Starting Client Login Server... ";

	WSADATA WsaData;
	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0)
	{
		cout << "Fail" << endl;
		cout << "Error On StartUp : " << GetLastError() << endl;
		system("pause");
		exit(-1);
	}

	SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET)
	{
		cout << "Fail" << endl;
		cout << "ListenSocket Error : " << GetLastError() << endl;
		system("pause");
		exit(-1);
	}

	SOCKADDR_IN ListenSockAddr;
	memset(&ListenSockAddr, 0, sizeof(ListenSockAddr));
	ListenSockAddr.sin_family = AF_INET;
	ListenSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ListenSockAddr.sin_port = htons(8881);

	int BindResult = _WINSOCK2API_::bind(ListenSocket, (SOCKADDR*)&ListenSockAddr, sizeof(ListenSockAddr));
	if (BindResult == SOCKET_ERROR)
	{
		cout << "Fail" << endl;
		cout << "Bind Error : " << GetLastError() << endl;
		system("pause");
		exit(-1);
	}

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cout << "Fail" << endl;
		cout << "Listen Error : " << GetLastError() << endl;
		system("pause");
		exit(-1);
	}

	cout << "Success!" << endl;

	while (true)
	{
		SOCKADDR_IN ClientSocketAddr;
		memset(&ClientSocketAddr, 0, sizeof(ClientSocketAddr));
		int ClientSockAddrLength = sizeof(ClientSocketAddr);

		SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSocketAddr, &ClientSockAddrLength);
		if (ClientSocket == INVALID_SOCKET)
		{
			cout << "Accept Error : " << GetLastError() << endl;
			continue;
		}

		char IP[1024] = { 0, };
		inet_ntop(AF_INET, &ClientSocketAddr.sin_addr.s_addr, IP, 1024);
		cout << "[" << (int)ClientSocket << "] Connected!" << endl;

		// create thread
		_beginthreadex(nullptr, 0, ServerThread, (void*)&ClientSocket, 0, nullptr);
	}

	// Clean Up
	closesocket(ListenSocket);
	WSACleanup();

	delete Sql_Connection;

	system("pause");

	return 0;
}
