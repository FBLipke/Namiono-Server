#include <Namiono-Server.h>
#include <Namiono-Service.h>
#include <sqlite3.h>
#pragma once

namespace Namiono
{
	namespace Common
	{
		EXPORT int _main(int argc, const char* argv);
		EXPORT void Handle_Request(const ServiceType* servicetype, const std::string& ident,
			const struct sockaddr_in* remote, const char* buffer, const _SIZET length);
	}
}
