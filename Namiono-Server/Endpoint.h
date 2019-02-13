#pragma once
#include <Namiono-Server.h>

class Endpoint
{
public:
	__declspec(dllexport) Endpoint() {};
	__declspec(dllexport) Endpoint(const ServiceType servicetype, const _IPADDR address,
		const unsigned short port, const int buffersize = 16384);

	__declspec(dllexport) ~Endpoint();

	__declspec(dllexport)_IPADDR& Get_Address();

	__declspec(dllexport) _SOCKET* Get_Socket();

	__declspec(dllexport) ServiceType* Get_ServiceType();
	__declspec(dllexport) char* Get_Buffer();
	__declspec(dllexport) bool Initialize();
	__declspec(dllexport) bool IsListening();
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