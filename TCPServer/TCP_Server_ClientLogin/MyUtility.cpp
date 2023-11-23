#include "MyUtility.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

bool MyUtility::GetConfigFromFile(string& OutServer, string& OutUserName, string& OutPassword)
{
	ifstream ConfigFile("config.txt");
	if (!ConfigFile.is_open())
	{
		cout << "Error: Could not open config file." << endl;
		return false;
	}

	string Line;
	size_t Start;
	size_t End;
	while (getline(ConfigFile, Line)) {
		if (Start = Line.find("Server = ") != string::npos) {
			End = Line.find('\n', Start);
			OutServer = Line.substr(Start + 8, End);		// Extract server information
		}
		else if (Start = Line.find("Username = ") != string::npos)
		{
			End = Line.find('\n', Start);
			OutUserName = Line.substr(Start + 10, End);	// Extract username
		}
		else if (Start = Line.find("Password = ") != string::npos)
		{
			End = Line.find('\n', Start);
			OutPassword = Line.substr(Start + 10, End);	// Extract password
		}
	}
	ConfigFile.close();
	return true;

}

string MyUtility::MultibyteToUtf8(const string& str) {
	int nLen = str.size();
	wchar_t warr[1024];
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str.c_str(), -1, warr, 1024);
	char carr[1024];
	memset(carr, '\0', sizeof(carr));
	WideCharToMultiByte(CP_UTF8, 0, warr, -1, carr, 1024, NULL, NULL);
	return carr;
}

string MyUtility::Utf8ToMultibyte(const string& str) {
	wchar_t warr[1024];
	int nLen = str.size();
	memset(warr, '\0', sizeof(warr));
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, warr, 1024);
	char carr[1024];
	memset(carr, '\0', sizeof(carr));
	WideCharToMultiByte(CP_ACP, 0, warr, -1, carr, 1024, NULL, NULL);
	return carr;
}

vector<string> MyUtility::ParsingString(const string& TargetStr, const char& TargetChar)
{
	vector<string> Data;

	istringstream Iss(TargetStr);
	string token;
	for (int i = 0; getline(Iss, token, TargetChar); i++)
	{
		Data[i] = token;
	}

	return Data;
}
