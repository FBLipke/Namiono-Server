#pragma once
#include <Namiono-Common.h>

class Service
{
public:
	EXPORT Service() = default;
	EXPORT virtual ~Service();

	EXPORT virtual void Handle_Service_Request(Server* server,
		const std::string& ident, Client* client, Packet* packet) = 0;

	EXPORT virtual void Update();
private:
	
};

EXPORT Service* Get_Service(const ServiceType& servicetype);
EXPORT Service* Add_Service(Server* server, const ServiceType& servicetype);
EXPORT void Remove_Service(const ServiceType& servicetype);