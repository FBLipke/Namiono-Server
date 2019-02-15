#include <Namiono-Server.h>

Server::Server()
{
	this->isRunning = false;
#ifdef _WIN32
	WSADATA wsa;
	int retval = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (retval != 0)
		printf("Failed to start Winsock! (%d)\n", WSAGetLastError());
#endif // _WIN32
}

Server::~Server()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

_IPADDR& Server::Get_IPAddress(const std::string & ident)
{
	return endpoints.at(ident)->Get_Address();
}

bool Server::Initialize()
{
	bool retval = true;

	for (auto & endpoint : endpoints)
	{
        endpoint.second.get()->Initialize();
	}

	return retval;
}

void Server::Update()
{
	for (auto & endp  : endpoints)
	{
		if (!endp.second->IsListening())
		{
			printf("Removing (unused) Endpoint %s....\n", endp.first.c_str());
			endpoints.erase(endp.first);
		}
	}

	for (auto & client : clients)
	{
		if (client.second == nullptr)
			endpoints.erase(client.first);

		if (client.second->DHCP->Get_State() == DHCP_ABORT ||
			client.second->DHCP->Get_State() == DHCP_WAITING ||
			client.second->DHCP->Get_State() == DHCP_DONE)
		{
			printf("Removing DHCP Client \"%s\"...\n", client.first.c_str());
			endpoints.erase(client.first);
			continue;
		}

		if (client.second->TFTP->GetTFTPState() == TFTP_ERROR ||
			client.second->TFTP->GetTFTPState() == TFTP_DONE)
		{
			printf("Removing TFTP Client \"%s\"...\n", client.first.c_str());
			endpoints.erase(client.first);
			continue;
		}
	}
}

void Server::Remove_Endpoint(const std::string& ident)
{
	this->endpoints.erase(ident);
}

void Server::Remove_Client(const std::string& ident)
{
	this->clients.erase(ident);
}

bool Server::Has_Client(const std::string& ident)
{
	return (clients.find(ident) != clients.end() && clients.size() != 0);
}

Client* Server::Get_Client(const ServiceType servicetype, const std::string& ident, const sockaddr_in& hint)
{
	std::string id = std::string(inet_ntoa(hint.sin_addr));

	if (Has_Client(id))
	{
		this->clients.at(id)->Set_Hint(hint);
		return this->clients.at(id).get();
	}

	return nullptr;
}

std::string & Server::Get_Hostname()
{
	return this->hostname;
}

bool Server::IsRunning()
{
	return isRunning;
}

void Server::Add_Endpoint(const ServiceType servicetype, const ServerMode servermode)
{
	char suffix[4];
	ClearBuffer(suffix, sizeof suffix);
	unsigned short _port;
	char hname[64];
	ClearBuffer(&hname, sizeof hname);
	unsigned long dwBufLen = 0;
	_IPADDR netmask = 0;
	_IPADDR ADDRESS = 0;
	std::string ident = "";
#ifdef _WIN32
	dwBufLen = sizeof(IP_ADAPTER_INFO);

	IP_ADAPTER_INFO AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == 111L)
	{
		free(AdapterInfo);
		AdapterInfo = (IP_ADAPTER_INFO*)malloc(dwBufLen);
	}

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == 0)
	{
#else
    struct sockaddr_in *sa;
	ifaddrs *AdapterInfo, *pAdapterInfo;
	if (getifaddrs(&AdapterInfo) == 0)
	{
#endif
		pAdapterInfo = AdapterInfo;
		do
		{
			sprintf(suffix, "%d", static_cast<unsigned char>(endpoints.size()));

			switch (servicetype)
			{
			case DHCP_SERVER:
				_port = 67;
				ident = "DHCP-Proxy";
				break;
			case BINL_SERVER:
			case BOOTP_SERVER:
				_port = 4011;
				ident = "BINL-Server";
				break;
			case TFTP_SERVER:
				_port = 69;
				ident = "TFTP-Server";
				break;
			}

			unsigned char _hwlen = 6;
			unsigned char _hwtype = 1;
#ifdef _WIN32
			netmask = inet_addr(pAdapterInfo->IpAddressList.IpMask.String);
			ADDRESS = inet_addr(pAdapterInfo->IpAddressList.IpAddress.String);
#else
			netmask = inet_addr("255.255.255.0");
            sa = (struct sockaddr_in *) pAdapterInfo->ifa_addr;
			ADDRESS = sa->sin_addr.s_addr;

			if (pAdapterInfo->ifa_addr->sa_family == AF_INET) // Only Ethernet is allowed for now.
			{
#endif
				std::string _ident = ident + "(" + std::string(suffix) + ")";
				endpoints.emplace(_ident, new Endpoint(servicetype, ADDRESS, _port, 16384));
#ifdef _WIN32
			pAdapterInfo = pAdapterInfo->Next;
#else
			}

			pAdapterInfo = pAdapterInfo->ifa_next;
#endif
		} while (pAdapterInfo);
	}
#ifdef _WIN32
	free(AdapterInfo);
#else
	freeifaddrs(AdapterInfo);
#endif
	gethostname(hname, sizeof hname);
	this->hostname = std::string(hname);
}

Client* Server::Add_Client(const ServiceType servicetype, const sockaddr_in* remote)
{
	std::string id = std::string(inet_ntoa(remote->sin_addr));

	if (!Has_Client(id))
		this->clients.emplace(id, new Client(id, servicetype, *remote));

	return clients.at(id).get();
}

int Server::Listen(const std::function<void(const ServiceType*,
	const std::string&, const struct sockaddr_in*, const char*, _SIZET)> callback)
{
	this->isRunning = true;
	int retval = SOCKET_ERROR;
	timeval timeout;


	printf("[I] Waiting for requests...!\n\n");
#ifdef __WIN32
	while (this->endpoints.size() > 0)
	{
		FD_ZERO(&_fd_read);
		FD_ZERO(&_fd_except);

		timeout.tv_sec = 0;
		timeout.tv_usec = 2;
		for (auto & endpoint : endpoints)
		{
			if (!endpoint.second.get()->IsListening())
			{
				Remove_Endpoint(endpoint.first);
				continue;
			}

			FD_SET(*endpoint.second.get()->Get_Socket(), &_fd_read);
			FD_SET(*endpoint.second.get()->Get_Socket(), &_fd_except);
		}

		retval = select(0, &_fd_read, &_fd_write, &_fd_except, &timeout);
		if (retval == SOCKET_ERROR)
		{
			printf("Error select(): %s\n", strerror(errno));
    	    break;
        }



		if (retval == 0)
			continue;
		printf("epic...\n");
		for (auto & endpoint : endpoints)
		{
			printf("after select...\n");

			if (!endpoint.second.get()->IsListening())
				continue;

			if (FD_ISSET(*endpoint.second.get()->Get_Socket(), &_fd_read))
			{
				sockaddr_in remote;
#ifdef _WIN32
				int fromlen = sizeof(remote);
#else
				socklen_t fromlen = sizeof(remote);
#endif
				memset(&remote, 0, sizeof remote);

				retval = recvfrom(*endpoint.second.get()->Get_Socket(),
					endpoint.second.get()->Get_Buffer(), 16384, 0,
					(struct sockaddr*)&remote, &fromlen);

				if (retval == SOCKET_ERROR)
				{
					printf("Error select(): %s\n", strerror(errno));
					break;
				}

				std::thread _notify(callback, endpoint.second.get()->Get_ServiceType(),
					endpoint.first, &remote, endpoint.second.get()->Get_Buffer(), retval);

				_notify.join();
				FD_CLR(*endpoint.second.get()->Get_Socket(), &_fd_read);
			}

			if (FD_ISSET(*endpoint.second.get()->Get_Socket(), &_fd_except))
			{
				int errNum = 0;
#ifdef _WIN32
				int len = sizeof(errNum);
#else
				socklen_t len = sizeof(errNum);
#endif
				retval = getsockopt(*endpoint.second.get()->Get_Socket(),
					SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&errNum), &len);

				if (errNum != 0)
				{
					printf("[E] Socket Error: %d\n", errNum);
					break;
				}

				FD_CLR(*endpoint.second.get()->Get_Socket(), &_fd_except);
			}
		}
	} // while

#else
	while(endpoints.size() > 0)
	{
		FD_ZERO(&_fd_read);
		FD_ZERO(&_fd_except);

		for (auto & endpoint : endpoints)
		{
			// Clear brocken Sockets...
			if (!endpoint.second.get()->IsListening())
			{
				Remove_Endpoint(endpoint.first);
				continue;
			}

			FD_SET(*endpoint.second.get()->Get_Socket(), &_fd_read);
			FD_SET(*endpoint.second.get()->Get_Socket(), &_fd_except);
		}

		for (auto & endpoint : endpoints)
		{
			timeout.tv_sec = 0;
			timeout.tv_usec = 2;

			retval = select(*endpoint.second.get()->Get_Socket() + 1, &_fd_read, NULL, NULL, &timeout);
			if (retval == SOCKET_ERROR)
			{
				continue;
			}

			if (retval == 0)
				continue;

			if (FD_ISSET(*endpoint.second.get()->Get_Socket(), &_fd_read))
			{
				sockaddr_in remote;
#ifdef _WIN32
				int fromlen = sizeof(remote);
#else
				socklen_t fromlen = sizeof(remote);
#endif
				memset(&remote, 0, sizeof remote);

				retval = recvfrom(*endpoint.second.get()->Get_Socket(),
					endpoint.second.get()->Get_Buffer(), 16384, 0,
					(struct sockaddr*)&remote, &fromlen);

				if (retval == SOCKET_ERROR)
				{
					printf("Error select(): %s\n", strerror(errno));
					break;
				}

				std::thread _notify(callback, endpoint.second.get()->Get_ServiceType(),
					endpoint.first, &remote, endpoint.second.get()->Get_Buffer(), retval);

				_notify.join();
				FD_CLR(*endpoint.second.get()->Get_Socket(), &_fd_read);
			}

			if (FD_ISSET(*endpoint.second.get()->Get_Socket(), &_fd_except))
			{
				int errNum = 0;
#ifdef _WIN32
				int len = sizeof(errNum);
#else
				socklen_t len = sizeof(errNum);
#endif
				retval = getsockopt(*endpoint.second.get()->Get_Socket(),
					SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&errNum), &len);

				if (errNum != 0)
				{
					printf("[E] Socket Error: %d\n", errNum);
					break;
				}

				FD_CLR(*endpoint.second.get()->Get_Socket(), &_fd_except);
			}
		}
	}
#endif

	this->Shutdown();

	return retval;
}

int Server::Send(const std::string& ident, Client* client, Packet* packet)
{
	int retval = SOCKET_ERROR;

	retval = sendto(*this->endpoints.at(ident).get()->Get_Socket(),
		packet->Get_Buffer(), packet->get_Length(), 0, (const sockaddr*)&*client->Get_Hint(), sizeof(*client->Get_Hint()));

	return retval;
}

void Server::Shutdown()
{
	this->isRunning = false;

	if (this->endpoints.size() != 0)
		this->endpoints.clear();
}
