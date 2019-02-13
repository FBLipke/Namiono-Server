#pragma once
#include <Namiono-Server.h>

class Server
{
public:
	__declspec(dllexport) Server();
	__declspec(dllexport) ~Server();
	__declspec(dllexport) _IPADDR& Get_IPAddress(const std::string & ident);
	__declspec(dllexport) void Add_Endpoint(const ServiceType servicetype, const ServerMode servermode);

	__declspec(dllexport) Client* Add_Client(const ServiceType servicetype, const sockaddr_in* remote);

	__declspec(dllexport) bool Initialize();

	__declspec(dllexport) void Remove_Endpoint(const std::string & ident);
	__declspec(dllexport) void Remove_Client(const std::string & ident);

	__declspec(dllexport) bool Has_Client(const std::string & ident);
	__declspec(dllexport) bool IsRunning();

	__declspec(dllexport) Client* Get_Client(const ServiceType servicetype, const std::string& ident, const sockaddr_in& hint);
	__declspec(dllexport) std::string& Get_Hostname();

	__declspec(dllexport) int Listen(const std::function<void(const ServiceType* servicetype,
		const std::string&, const sockaddr_in*, const char*, int)> callback);
	
	__declspec(dllexport) int Send(const std::string& ident, Client* client, Packet* packet);

	__declspec(dllexport) void Shutdown();
	__declspec(dllexport) void Update();

private:
	std::map<std::string, std::unique_ptr<Endpoint>> endpoints;
	std::map<std::string, std::unique_ptr<Client>> clients;
	std::string hostname;
	FD_SET _fd_read;
	FD_SET _fd_except;
	FD_SET _fd_write;
	bool isRunning;
};
