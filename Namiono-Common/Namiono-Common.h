#include <Namiono-Server.h>
#include <Namiono-Service.h>
#include <sqlite3.h>
#pragma once



namespace Namiono
{
	namespace Common
	{
		__declspec(dllexport) void Update();
		__declspec(dllexport) int _main(int argc, const char* argv);
		__declspec(dllexport) void Handle_Request(const ServiceType* servicetype, const std::string& ident,
			const sockaddr_in* remote, const char* buffer, const _SIZE_T length);
	}
}
