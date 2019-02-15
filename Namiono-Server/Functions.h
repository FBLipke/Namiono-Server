#pragma once
#include <Namiono-Server.h>

class Functions
{
public:
	static EXPORT std::string AddressStr(const _IPADDR ip);
	static EXPORT std::string Replace(std::string& str, const std::string& from, const std::string& to);
	static EXPORT std::vector<std::string> Split(const std::string& str, const std::string& token);
	static EXPORT bool Compare(const char* p1, const char* p2, const _SIZET length);

	static EXPORT _SIZET Strip(const char* buffer, const _SIZET buflen);
	static EXPORT std::string AsString(const _SIZET input);
	static EXPORT void ExtractString(const char* buf, const size_t& size, char* out);
	static EXPORT bool FileExist(const char* Filename);
	static EXPORT std::string Combine(std::string p1, std::string p2);
	static EXPORT std::string __getcwd();
	static EXPORT bool __has_endingslash(const std::string& p);
	static EXPORT std::string __pathSeperatorChar();
	static EXPORT std::string __fixpath(const std::string& p);
	static EXPORT std::string __replaceSlash(const std::string& p);
	static EXPORT std::string __resolveMacro(const std::string& p);
};

