#pragma once
#include <Namiono-Server.h>

class Endpoint
{
public:
	EXPORT Endpoint(const ServiceType servicetype, const _IPADDR address,
		const unsigned short port, const int buffersize = 16384);

	EXPORT ~Endpoint();

	EXPORT _IPADDR& Get_Address();

	EXPORT _SOCKET* Get_Socket();

	EXPORT ServiceType* Get_ServiceType();
	EXPORT char* Get_Buffer();
	EXPORT bool Initialize();
	EXPORT bool IsListening();
private:
	unsigned short _port;
	int optval = 1;
	_IPADDR _address;
	_SOCKET _socket;
	sockaddr_in listener;
	std::unique_ptr<char> buffer;
	int buffersize;
	bool isListening;
	ServiceType serviceType;
};