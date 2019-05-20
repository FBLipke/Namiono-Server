#pragma once
#include <Namiono/Namiono.h>

class Functions
{
public:
	static std::string AddressStr(const _IPADDR ip, const int family = 2);
	static std::string Replace(std::string& str, const std::string& from, const std::string& to);
	static std::vector<std::string> Split(const std::string& str, const std::string& token);
	static bool Compare(const char* p1, const char* p2, const _SIZET length);

	static _SIZET Strip(const char* buffer, const _SIZET buflen);
	static std::string AsString(const _SIZET input);
	static void ExtractString(const char* buf, const size_t& size, char* out);
	static bool FileExist(const char* Filename);
};

