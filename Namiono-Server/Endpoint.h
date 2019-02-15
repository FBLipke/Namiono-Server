#pragma once
#include <Namiono-Server.h>

class Endpoint
{
public:
	EXPORT Endpoint(const ServiceType servicetype, const _IPADDR address,
		const unsigned short port, const int buffersize);

	EXPORT ~Endpoint();

	EXPORT _IPADDR& Get_Address();
	EXPORT sockaddr_in* Get_Listener();
	EXPORT _SOCKET* Get_Socket();

	EXPORT ServiceType* Get_ServiceType();
	EXPORT char* Get_Buffer();
	EXPORT bool Initialize();
	EXPORT bool IsListening();
private:
	unsigned short _port;
#ifdef _WIN32
	int optval = 1;
#else
	socklen_t optval = 1;
#endif
	_IPADDR _address;
	_SOCKET _socket;
	sockaddr_in listener;
	std::unique_ptr<char> buffer;
	int buffersize;
	bool isListening;
	ServiceType serviceType;
};
