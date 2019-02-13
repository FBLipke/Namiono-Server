#pragma once
#include <Namiono-Server.h>

class Server
{
public:
	EXPORT Server();
	EXPORT ~Server();
	EXPORT _IPADDR& Get_IPAddress(const std::string & ident);
	EXPORT void Add_Endpoint(const ServiceType servicetype, const ServerMode servermode);

	EXPORT Client* Add_Client(const ServiceType servicetype, const sockaddr_in* remote);

	EXPORT bool Initialize();

	EXPORT void Remove_Endpoint(const std::string & ident);
	EXPORT void Remove_Client(const std::string & ident);

	EXPORT bool Has_Client(const std::string & ident);
	EXPORT bool IsRunning();

	EXPORT Client* Get_Client(const ServiceType servicetype, const std::string& ident, const sockaddr_in& hint);
	EXPORT std::string& Get_Hostname();

	EXPORT int Listen(const std::function<void(const ServiceType* servicetype,
		const std::string&, const sockaddr_in*, const char*, int)> callback);
	
	EXPORT int Send(const std::string& ident, Client* client, Packet* packet);

	EXPORT void Shutdown();
	EXPORT void Update();

private:
	std::map<std::string, std::unique_ptr<Endpoint>> endpoints;
	std::map<std::string, std::unique_ptr<Client>> clients;
	std::string hostname;
	FD_SET _fd_read;
	FD_SET _fd_except;
	FD_SET _fd_write;
	bool isRunning;
};
