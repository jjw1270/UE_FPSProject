#pragma once

#include <Windows.h>
#include <string>
#include <vector>

using namespace std;

class MyUtility
{
public:
	static bool GetConfigFromFile(string& OutServer, string& OutUserName, string& OutPassword);

	static string MultibyteToUtf8(const string& str);

	static string Utf8ToMultibyte(const string& str);

	static vector<string> ParsingString(string TargetStr, const char& TargetChar);

};
