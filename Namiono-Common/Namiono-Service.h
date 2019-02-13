#pragma once
#include <Namiono-Common.h>

class Service
{
public:
	__declspec(dllexport) Service() = default;
	__declspec(dllexport) virtual ~Service();

	__declspec(dllexport) virtual void Handle_Service_Request(Server* server,
		const std::string& ident, Client* client, Packet* packet) = 0;

	__declspec(dllexport) virtual void Update();
private:
	
};

__declspec(dllexport) Service* Get_Service(const ServiceType& servicetype);
__declspec(dllexport) Service* Add_Service(Server* server, const ServiceType& servicetype);
__declspec(dllexport) void Remove_Service(const ServiceType& servicetype);