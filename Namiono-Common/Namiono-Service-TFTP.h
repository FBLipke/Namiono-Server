#pragma once
#include <Namiono-Service.h>
class Namiono_Service_TFTP :
	public Service
{
public:
	Namiono_Service_TFTP();
	~Namiono_Service_TFTP();

	virtual void Update() override;
	virtual void Handle_Service_Request(Server* server, const std::string& ident, Client* client, Packet* packet) override;
private:
	void Handle_TFTP_ERR(Server* server, const std::string & ident, Client * client, Packet * packet);
	void Handle_TFTP_RRQ(Server* server, const std::string& ident, Client* client, Packet* packet);
	void Handle_TFTP_ACK(Server* server, const std::string& ident, Client* client, Packet* packet);
};

