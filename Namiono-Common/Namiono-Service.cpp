#include <Namiono-Common.h>
#include <Namiono-Service-DHCP.h>
#include <Namiono-Service-TFTP.h>
#include <Namiono-Service-BINL.h>

Service::~Service()
{
}

std::map<ServiceType, std::unique_ptr<Service>> services;

void Service::Handle_Service_Request(Server* server,
	const std::string& ident, Client* client, Packet* packet)
{
	printf("[W] %s: There is no Service for this Request!\n", ident.c_str());
	server->Remove_Client(client->Get_Ident());
}

void Service::Update()
{

}

Service* Get_Service(const ServiceType& servicetype)
{
	return services.at(servicetype).get();
}

Service* Add_Service(Server* server, const ServiceType& servicetype)
{
	if (services.find(servicetype) == services.end())
	{
		switch (servicetype)
		{
		case DHCP_SERVER:
		case BOOTP_SERVER:
			services.insert(std::pair<ServiceType,
				std::unique_ptr<Service>>(servicetype, new Namiono_Service_DHCP(server)));
			break;
		case TFTP_SERVER:
			services.insert(std::pair<ServiceType,
				std::unique_ptr<Service>>(servicetype, new Namiono_Service_TFTP()));
			break;
		case BINL_SERVER:
			services.insert(std::pair<ServiceType,
				std::unique_ptr<Service>>(servicetype, new Namiono_Service_BINL()));
			break;
		default:
			break;
		}
	}

	return Get_Service(servicetype);
}

void Remove_Service(const ServiceType & servicetype)
{
	services.erase(servicetype);
}
