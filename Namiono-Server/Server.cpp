#include <Namiono-Server.h>

Server::Server()
{
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
	bool retval = false;

	for (auto & endpoint : endpoints)
	{
		retval = endpoint.second.get()->Initialize();
		if (!retval)
			printf("Bind failed!\n");
	}

	return retval;
}

void Server::Update()
{
	for (auto & endp  : endpoints)
	{
		printf("Updating Endpoint %s....\n", endp.first.c_str());

		if (!endp.second->IsListening())
		{
			endpoints.erase(endp.first);
		}
	}

	for (auto & client : clients)
	{
		if (client.second == nullptr)
			endpoints.erase(client.first);

		printf("Updating client %s....\n", client.first.c_str());

		if (client.second->DHCP->Get_State() == DHCP_ABORT ||
			client.second->DHCP->Get_State() == DHCP_WAITING ||
			client.second->DHCP->Get_State() == DHCP_DONE)
		{
			endpoints.erase(client.first);
			continue;
		}

		if (client.second->TFTP->GetTFTPState() == TFTP_ERROR ||
			client.second->TFTP->GetTFTPState() == TFTP_DONE)
		{
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
	char mac[32];
	ClearBuffer(mac, sizeof mac);
	unsigned short _port;
	char hname[64];
	ClearBuffer(&hname, sizeof hname);
	unsigned long dwBufLen = sizeof(IP_ADAPTER_INFO);
	_IPADDR netmask;
	_IPADDR ADDRESS;
	std::string ident;

	// Bind each Endpoint on each Adapter... 
	auto AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == 111L)
	{
		free(AdapterInfo);
		AdapterInfo = (IP_ADAPTER_INFO*)malloc(dwBufLen);
	}

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == 0)
	{
		auto pAdapterInfo = AdapterInfo;
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

			auto _hwlen = 6;
			auto _hwtype = 1;

			sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
				pAdapterInfo->Address[0], pAdapterInfo->Address[1],
				pAdapterInfo->Address[2], pAdapterInfo->Address[3],
				pAdapterInfo->Address[4], pAdapterInfo->Address[5]);

			netmask = inet_addr(pAdapterInfo->IpAddressList.IpMask.String);
			ADDRESS = inet_addr(pAdapterInfo->IpAddressList.IpAddress.String);

			_hwtype = pAdapterInfo->Type == 6 ? 1 : 0;
			_hwlen = pAdapterInfo->AddressLength;

			if (_hwtype == 1) // Only Ethernet is allowed for now.
			{
				auto _ident = ident + "(" + std::string(suffix) + ")";

				endpoints.try_emplace(_ident, new Endpoint(servicetype, ADDRESS, _port));
			}

			pAdapterInfo = pAdapterInfo->Next;
		} while (pAdapterInfo);
	}

	free(AdapterInfo);

	gethostname(hname, sizeof hname);
	this->hostname = std::string(hname);
}

Client* Server::Add_Client(const ServiceType servicetype, const sockaddr_in* remote)
{
	std::string id = std::string(inet_ntoa(remote->sin_addr));

	if (!Has_Client(id))
		this->clients.insert(std::pair<std::string, std::unique_ptr<Client>>
			(id, new Client(id, servicetype, *remote)));

	return clients.at(id).get();
}

int Server::Listen(const std::function<void(const ServiceType* servicetype,
	const std::string&, const sockaddr_in*, const char*, int)> callback)
{
	this->isRunning = true;
	int retval = SOCKET_ERROR;
	
	while (this->endpoints.size() > 0)
	{
		FD_ZERO(&_fd_read);
		FD_ZERO(&_fd_except);

		for (auto & endpoint : endpoints)
		{
			if (!endpoint.second.get()->IsListening())
			{
				Remove_Endpoint(endpoint.first);
				break;
			}

			FD_SET(*endpoint.second.get()->Get_Socket(), &_fd_read);
			FD_SET(*endpoint.second.get()->Get_Socket(), &_fd_except);
		}

		retval = select(0, &_fd_read, &_fd_write, &_fd_except, NULL);
		if (retval == SOCKET_ERROR)
			break;

		if (retval == 0)
			continue;

		for (auto & endpoint : endpoints)
		{
			if (!endpoint.second.get()->IsListening())
				continue;

			if (FD_ISSET(*endpoint.second.get()->Get_Socket(), &_fd_read))
			{
				sockaddr_in remote;
				int fromlen = sizeof(remote);
				memset(&remote, 0, sizeof remote);

				retval = recvfrom(*endpoint.second.get()->Get_Socket(),
					endpoint.second.get()->Get_Buffer(), 16384, 0,
					(struct sockaddr*)&remote, &fromlen);

				if (retval == SOCKET_ERROR)
					break;
				std::thread _notify(callback, endpoint.second.get()->Get_ServiceType(),
					endpoint.first, &remote, endpoint.second.get()->Get_Buffer(), retval);

				_notify.join();
				FD_CLR(*endpoint.second.get()->Get_Socket(), &_fd_read);
			}

			if (FD_ISSET(*endpoint.second.get()->Get_Socket(), &_fd_except))
			{
				int errNum = 0;
				int len = sizeof(errNum);

				retval = getsockopt(*endpoint.second.get()->Get_Socket(),
					SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&errNum), &len);

				if (errNum != 0 || retval == SOCKET_ERROR)
				{
					printf("Socket Error: %d\n", errNum);
					break;
				}

				FD_CLR(*endpoint.second.get()->Get_Socket(), &_fd_except);
				break;
			}
		}
	}

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
