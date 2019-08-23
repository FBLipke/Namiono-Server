/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _CRT_SECURE_NO_WARNINGS 1
#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Network
	{
		Server::Server()
		{

		}

		Server::Server(std::vector<_IPADDR>* addrList, const std::string& rootDir,
			std::function<void(ServiceType, Server*, int, Client*, std::string, Packet*)> cb)
		{
			this->rootDir = rootDir;
			_IPADDR address = 0;
			this->callback = cb;

			std::vector<_USHORT> _ports;

			_ports.emplace_back(67);
			_ports.emplace_back(69);
			_ports.emplace_back(4011);

			if (_ports.size() == 0)
				return;

#ifdef _WIN32
			PIP_ADAPTER_INFO pAdapterInfo = nullptr;
			PIP_ADAPTER_INFO pAdapter = nullptr;
			_ULONG ulOutBufLen = 0;

			if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
			{
				free(pAdapterInfo);
				pAdapterInfo = static_cast<IP_ADAPTER_INFO*>(malloc(ulOutBufLen));
				if (pAdapterInfo == nullptr)
					return;
			}

			if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
			{
				pAdapter = pAdapterInfo;
				while (pAdapter)
				{
					address = inet_addr(pAdapter->IpAddressList.IpAddress.String);
					addrList->emplace_back(address);

					for (_SIZET i = 0; i < _ports.size(); i++)
					{
						Interfaces.emplace_back(address, static_cast<_USHORT>(Interfaces.size()), _ports.at(i));
					}

					pAdapter = pAdapter->Next;
				}

				pAdapter = nullptr;
			}

			if (pAdapterInfo)
				free(pAdapterInfo);

			pAdapterInfo = nullptr;
#else
			ifaddrs* ifap = nullptr, *ifa = nullptr;
			getifaddrs(&ifap);
			for (ifa = ifap; ifa; ifa = ifa->ifa_next)
			{
				if (ifa->ifa_addr && (ifa->ifa_addr->sa_family == AF_INET && ifa->ifa_addr->sa_family != AF_LOOPBACK))
				{
					sockaddr_in* sa = (struct sockaddr_in*) ifa->ifa_addr;
					address = sa->sin_addr.s_addr;
					addrList->emplace_back(address);

					for (_INT32 i = 0; i < ports.size(); i++)
					{
						Interfaces.emplace_back(address, static_cast<_USHORT>(Interfaces.size()), _ports.at(i));
					}
				}
			}

			freeifaddrs(ifap);
			ifap = nullptr;
#endif
			global_srv_ptr = this;
		}

		Server::~Server()
		{
		}

		bool Server::Init()
		{
			for (_SIZET i = 0; i < Interfaces.size(); i++)
			{
				Interfaces.at(i).Init();
			}

			return this->Interfaces.size() != 0;
		}

		void Server::Remove_Client(const std::string& ident)
		{
			std::string _id = ident;

			if (Has_Client(_id))
			{
				delete clients.at(_id);
				clients.at(_id) = nullptr;

				clients.erase(_id);
			}
		}

		bool Server::Start()
		{
			for (_SIZET i = 0; i < Interfaces.size(); i++)
			{
				Interfaces.at(i).Start();
			}

			started = this->Interfaces.size() != 0;
			return started;
		}

		void __Listen(Server* srv)
		{
			timeval time;
		
			FD_ZERO(&srv->Get_Read_Descriptors());

			for (_SIZET i = 0; i < srv->Get_Interfaces().size(); i++)
				FD_SET(srv->Get_Interfaces().at(i).Get_Socket(), &srv->Get_Read_Descriptors());

			fd_set copy_of_read = srv->Get_Read_Descriptors();

			while (srv->Is_Running())
			{
				time.tv_sec = 0;
				time.tv_usec = 2;

				copy_of_read = srv->Get_Read_Descriptors();

				const _INT32 count = select(static_cast<_INT32>(srv->Get_Interfaces().size() + 1), &copy_of_read, nullptr, nullptr, &time);

				for (_INT32 iS = 0; iS < count; iS++)
				{
					for (_SIZET iE = 0; iE < srv->Get_Interfaces().size(); iE++)
					{
						if (FD_ISSET(srv->Get_Interfaces().at(iE).Get_Socket(), &copy_of_read))
						{
							if (!copy_of_read.fd_array[iS] == srv->Get_Interfaces().at(iE).Get_Socket())
								continue;

							FD_CLR(srv->Get_Interfaces().at(iE).Get_Socket(), &copy_of_read);

							socklen_t remote_len = 0;
							sockaddr_in remote;
							ClearBuffer(&remote, sizeof remote);

							char* buffer = new char[16385];
							ClearBuffer(buffer, 16385);

							_SIZET bytes = SOCKET_ERROR;
							remote_len = sizeof remote;
							bytes = recvfrom(srv->Get_Interfaces().at(iE).Get_Socket(), buffer, 16385, 0,
								reinterpret_cast<struct sockaddr*>(&remote), &remote_len);

							if (bytes == SOCKET_ERROR)
							{
								printf("[E] recvfrom(): %s\n", strerror(errno));
								continue;
							}

							ServiceType t;

							switch (srv->Get_Interface(iE)->Get_Port())
							{
							case 67:
							case 4011:
								t = DHCP_SERVER;
								break;
							case 69:
								t = TFTP_SERVER;
								break;
							default:
								break;
							}
							Packet* request = new Packet(t, buffer, &bytes);
							srv->callback(t, srv, srv->Get_Interfaces().at(iE).Get_Id(),
								srv->Add_Client(t, remote), srv->Get_TFTP_Directory(), request);
							delete[] buffer;
							buffer = nullptr;

							delete request;
							request = nullptr;
						}
					}
				}
			}
		}

		int Server::Get_Interface_by_Mac(const std::string& mac)
		{
			for (_SIZET i = 0; i < Interfaces.size(); i++)
			{
				if (Interfaces.at(i).Has_ARPEntry(mac))
				{
#ifdef _DEBUG
					printf("[D] MAC Entry found at Interface %d\n", Interfaces.at(i).Get_Id());
#endif
					return Interfaces.at(i).Get_Id();
				}
			}

			return -1;
		}

		void Server::Send(int iface, Client* client)
		{
			switch (client->dhcp->Get_State())
			{
			case DHCP_RELAY:
				this->Get_Interfaces().at(iface).Send(client->Get_Relay_Hint(), client->response);
				break;
			default:
				this->Get_Interfaces().at(iface).Send(client->Get_Client_Hint(), client->response);
				break;
			}
		}

		Iface* Server::Get_Interface(const int& id)
		{
			return &this->Interfaces.at(id);
		}

		bool Server::Listen(std::vector<std::thread>* threads)
		{
			threads->emplace_back(__Listen, global_srv_ptr);
			return threads->size() != 0;
		}

		bool Server::Close()
		{
			for (_SIZET i = 0; i < Interfaces.size(); i++)
			{
				Interfaces.at(i).Close();
			}

			if (this->Interfaces.size() != 0)
				this->Interfaces.clear();

			return true;
		}

		bool Server::Has_Client(const std::string& key)
		{
			return clients.find(key) != clients.end();
		}

		std::string Server::Get_TFTP_Directory() const
		{
			return this->rootDir;
		}

		Client* Server::Add_Client(const ServiceType& t, const sockaddr_in& remote)
		{
			std::string addr = Functions::AddressStr(
				remote.sin_addr.s_addr, remote.sin_family);

			if (Has_Client(addr))
			{
				clients.at(addr)->Set_Port(remote.sin_port);
				return clients.at(addr);
			}

			clients.emplace(addr, new Client(t, Functions::AddressStr(remote.sin_addr.s_addr), remote, addr));

			return clients.at(addr);
		}

		std::vector<Iface>& Server::Get_Interfaces()
		{
			return this->Interfaces;
		}

		bool Server::Is_Running()
		{
			return this->started;
		}

		fd_set & Server::Get_Read_Descriptors()
		{
			return this->master_read;
		}
	}
}
