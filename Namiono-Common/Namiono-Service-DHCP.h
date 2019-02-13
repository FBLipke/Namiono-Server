#pragma once
#include <Namiono-Service.h>
class Namiono_Service_DHCP : public Service
{
public:
	virtual void Update() override;

	Namiono_Service_DHCP(Server* server);
	~Namiono_Service_DHCP();

	virtual void Handle_Service_Request(Server* server, const std::string & ident, Client* client, Packet* packet) override;
private:
	void Handle_DHCP_Discover(Server* server, const std::string& ident, Client* client, Packet* packet);
	void Handle_DHCP_Request(Server* server, const std::string& ident, Client* client, Packet* packet);
	void Create_BootServer_List(Server* server, const std::string& ident, std::vector<DHCP_Option>& vendorOps);
	void GenerateBootMenue(const Client* client, std::vector<DHCP_Option>* vendorOpts);
	void Handle_WDS_Options(Server* server, const std::string& ident, Client* client, Packet* response);
private:
	unsigned long requestId = 0;
	unsigned short id = 1;

	std::vector<BootServerEntry> Serverlist;
	std::vector<_IPADDR> addresses;
	sqlite3_stmt* stmt = NULL;
	const char* data = NULL;
	sqlite3* db = NULL;
};

