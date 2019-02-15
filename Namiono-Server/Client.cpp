#include <Namiono-Server.h>

Client::Client(const std::string& ident, const ServiceType servicetype, const sockaddr_in remote)
{
	this->Set_Hint(remote);

	this->serviceType = servicetype;
	this->ident = ident;
	this->TFTP = std::unique_ptr<TFTP_CLIENT>(new TFTP_CLIENT());
	this->DHCP = std::unique_ptr<DHCP_CLIENT>(new DHCP_CLIENT());
}

Client::~Client()
{
}

std::string Client::Get_Ident()
{
	return this->ident;
}

sockaddr_in* Client::Get_Hint()
{
	return &this->hint;
}

void Client::Set_Hint(const sockaddr_in hint)
{
	this->hint = hint;

	this->hint.sin_addr.s_addr = hint.sin_addr.s_addr != 0
		? hint.sin_addr.s_addr : INADDR_BROADCAST;
}
