#include <Namiono-Server.h>
#include <sys/fcntl.h>

Endpoint::Endpoint(const ServiceType servicetype, const _IPADDR address, const unsigned short port, const int buffersize)
{
	this->serviceType = servicetype;
	this->buffersize = buffersize;
	this->_address = address;
	this->_port = port;
	this->isListening = false;
	this->_socket = static_cast<_SOCKET>(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));
	fcntl(this->_socket, F_SETFL, O_NONBLOCK);
}

Endpoint::~Endpoint()
{
	if (closesocket(this->_socket) == 0)
		printf("[D] Endpoint \"%s\" closed!\n", inet_ntoa(this->listener.sin_addr));
}

_IPADDR& Endpoint::Get_Address()
{
	return this->_address;
}

_SOCKET* Endpoint::Get_Socket()
{
	return &this->_socket;
}

ServiceType* Endpoint::Get_ServiceType()
{
	return &this->serviceType;
}

char * Endpoint::Get_Buffer()
{
	return this->buffer.get();
}

sockaddr_in* Endpoint::Get_Listener()
{
	return &this->listener;
}

bool Endpoint::Initialize()
{
	int retval = SOCKET_ERROR;
	retval = setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));
	if (retval == SOCKET_ERROR)
	{
		printf("[E] Failed to enable \"REUSEADDRESS\"!\n");
		return retval;
	}
	retval = setsockopt(this->_socket, SOL_SOCKET, SO_BROADCAST, (const char*)&optval, sizeof(optval));

	if (retval == SOCKET_ERROR)
	{
		printf("[E] Failed to enable \"BROADCAST\"!\n");
		return retval;
	}

	this->listener.sin_addr.s_addr = this->_address;
	this->listener.sin_family = AF_INET;
	this->listener.sin_port = htons(this->_port);

	retval = bind(this->_socket, (const sockaddr*)&this->listener, sizeof this->listener);
	this->buffer = std::unique_ptr<char>(new char[this->buffersize]);
	if (retval == -1)
	{
		printf("Error bind(): %s\n", strerror(errno));
	}
	this->isListening = retval == 0;
	return this->IsListening();
}

bool Endpoint::IsListening()
{
	return this->isListening;
}
