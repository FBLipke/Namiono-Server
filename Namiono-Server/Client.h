#pragma once
#include <Namiono-Server.h>

class Client
{
public:
	__declspec(dllexport) Client(const std::string& ident, const ServiceType servicetype, const sockaddr_in remote);
	__declspec(dllexport) Client();
	__declspec(dllexport) ~Client();

	__declspec(dllexport) std::string Get_Ident();
	__declspec(dllexport) sockaddr_in * Get_Hint();
	__declspec(dllexport) void Set_Hint(const sockaddr_in hint);
	std::unique_ptr<DHCP_CLIENT> DHCP;
	std::unique_ptr<TFTP_CLIENT> TFTP;
private:
	sockaddr_in hint;
	ServiceType serviceType;

	std::string ident;
};

