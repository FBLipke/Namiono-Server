#pragma once
#include <Namiono-Server.h>

class Client
{
public:
	EXPORT Client(const std::string& ident, const ServiceType servicetype, const sockaddr_in remote);
	EXPORT ~Client();

	EXPORT std::string Get_Ident();
	EXPORT sockaddr_in* Get_Hint();
	EXPORT void Set_Hint(const sockaddr_in hint);
	std::unique_ptr<DHCP_CLIENT> DHCP;
	std::unique_ptr<TFTP_CLIENT> TFTP;
private:
	sockaddr_in hint;
	ServiceType serviceType;

	std::string ident;
};

