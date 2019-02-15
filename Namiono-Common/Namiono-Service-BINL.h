#pragma once
#include <Namiono-Service.h>
class Namiono_Service_BINL :
	public Service
{
public:
	Namiono_Service_BINL();
	~Namiono_Service_BINL();

	virtual void Handle_Service_Request(Server * server, const std::string & ident, Client * client, Packet * packet) override;
	void Handle_BINL_RQU(Server* server, const std::string& ident, Client* client, Packet* packet);
	void Handle_BINL_NEG(Server* server, const std::string& ident, Client* client, Packet* packet);
private:
};

