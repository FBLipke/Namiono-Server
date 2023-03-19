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

		void Server::Add_Interfaces()
		{
			if (this->Interfaces.size() != 0)
			{
				this->Interfaces.clear();
				this->addresses.clear();
			}

			_IPADDR address = 0;
			_IPADDR netmask = 0;
			_IPADDR gateway = 0;
			std::string _name = "";

			_USHORT index = 0;
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
					if (address == inet_addr("127.0.0.1") || address == 0)
					{
						pAdapter = pAdapter->Next;
						continue;
					}

					index = static_cast<_USHORT>(pAdapter->ComboIndex);
					_name = std::string(pAdapter->AdapterName);

					netmask = inet_addr(pAdapter->IpAddressList.IpMask.String);
					gateway = inet_addr(pAdapter->GatewayList.IpAddress.String);

					addresses.emplace_back(address);

					Add_Interface(_name, index, address, netmask, gateway);

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
				if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET)
				{
					address = ((struct sockaddr_in*)ifa->ifa_addr)->sin_addr.s_addr;

					if (address == inet_addr("127.0.0.1") || address == 0)
						continue;

					index = if_nametoindex(ifa->ifa_name);
					_name = std::string(ifa->ifa_name);

					netmask = ((struct sockaddr_in*)ifa->ifa_netmask)->sin_addr.s_addr;
					addresses.emplace_back(address);

					Add_Interface(_name, index, address, netmask, gateway);
				}
			}

			freeifaddrs(ifap);
			ifap = nullptr;
#endif
		}

		Server::Server(SETTINGS* settings, std::function<void(ServiceType, Server*, _USHORT, Client*, Packet*)> cb)
		{
			this->settings = settings;
			this->callback = cb;


			global_srv_ptr = this;
		}

		void Server::Add_Interface(const std::string& name, const _USHORT& index,
			const _IPADDR& address, const _IPADDR& netmask, const _IPADDR& gateway)
		{
			std::vector<_USHORT> _ports;

			_ports.emplace_back(67);
			_ports.emplace_back(69);
			_ports.emplace_back(4011);

			for (_SIZET i = 0; i < _ports.size(); i++)
			{
				ServiceType t = UNKNOWN_SERVICE;

				switch (_ports.at(i))
				{
				case 67:
					t = DHCP_SERVER;
					break;
				case 69:
					t = TFTP_SERVER;
					break;
				case 4011:
					t = BINL_SERVER;
					break;
				default:
					t = UNKNOWN_SERVICE;
					continue;
				}

				Interfaces.emplace_back(this->settings, t, name, index, address, netmask, gateway,
					static_cast<_USHORT>(Interfaces.size()), _ports.at(i));
			}
		}

		Server::~Server()
		{
		}

		bool Server::Init()
		{
			this->Add_Interfaces();

			for (_SIZET i = 0; i < Interfaces.size(); i++)
				Interfaces.at(i).Init();

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
				Interfaces.at(i).Start();

			started = this->Interfaces.size() != 0;
			return started;
		}

		void __AddSocketsToList(Server* srv)
		{
			for (Iface & iface : srv->Get_Interfaces())
			{
				if (!iface.isListening())
					continue;

				
				FD_SET(iface.Get_Socket(), &srv->Get_Read_Descriptors());
				FD_SET(iface.Get_Socket(), &srv->Get_Write_Descriptors());
				FD_SET(iface.Get_Socket(), &srv->Get_Except_Descriptors());

				printf("[I] Listening on [%d] %s:%d\n", iface.Get_Id(),
					Functions::AddressStr(iface.Get_IPAddress()).c_str(),
					iface.Get_Port());
			}
		}

		void __ClearSocketList(Server* srv)
		{
			FD_ZERO(&srv->Get_Read_Descriptors());
			FD_ZERO(&srv->Get_Write_Descriptors());
			FD_ZERO(&srv->Get_Except_Descriptors());
		}

		void __read(Iface* _interface, char* buffer, _SIZET length)
		{

		}

		void __Listen(Server* srv)
		{
#ifdef _WIN32
			timeval time;
#else
			timespec time;
#endif                        

			__ClearSocketList(srv);
			__AddSocketsToList(srv);

			fd_set copy_of_read = srv->Get_Read_Descriptors();
			fd_set copy_of_write = srv->Get_Write_Descriptors();
			fd_set copy_of_except = srv->Get_Except_Descriptors();


			bool handled = false;
			bool rescanInterfaces = false;

			while (srv->Is_Running())
			{
				time.tv_sec = 0;
#ifdef _WIN32
				time.tv_usec = 1;
#else
				time.tv_nsec = 1;
#endif

				copy_of_read = srv->Get_Read_Descriptors();
				copy_of_write = srv->Get_Write_Descriptors();
				copy_of_except = srv->Get_Except_Descriptors();

				_INT32 count = _select(srv->Get_Interfaces().size() + 1, &copy_of_read, NULL, NULL, NULL);

				if (count == 0)
					continue;

				if (count == SOCKET_ERROR)
				{
					rescanInterfaces = true;

					if (rescanInterfaces)
					{
						srv->Close();
						srv->Init();
						srv->Start();
						__AddSocketsToList(srv);

						rescanInterfaces = false;

						continue;
					}

					break;
				}


				for (_INT32 iS = 0; iS < count; iS++)
				{
					_USHORT iFaceID = 0;

					for (Iface & iface : srv->Get_Interfaces())
					{
						if (!iface.isListening())
						{
							FD_CLR(iface.Get_Socket(), &copy_of_read);
							FD_CLR(iface.Get_Socket(), &copy_of_write);
							FD_CLR(iface.Get_Socket(), &copy_of_except);

							iFaceID = iface.Get_Id();
							break;
						}

						if (FD_ISSET(iface.Get_Socket(), &copy_of_read))
						{
							sockaddr_in remote;
							ClearBuffer(&remote, sizeof remote);

							char* buffer = new char[16385];
							ClearBuffer(buffer, 16385);

							_SIZET bytes = SOCKET_ERROR;
							socklen_t remote_len = sizeof remote;
							bytes = recvfrom(iface.Get_Socket(), buffer, 16386, 0,
								reinterpret_cast<struct sockaddr*>(&remote), &remote_len);

							if (bytes == SOCKET_ERROR)
							{
								printf("[E] recvfrom(): %s\n", strerror(errno));
								break;
							}

							ServiceType t;

							switch (iface.Get_Port())
							{
							case 67:
								t = DHCP_SERVER;
								break;
							case 4011:
								t = BINL_SERVER;
								break;
							case 69:
								t = TFTP_SERVER;
								break;
							default:
								continue;
							}

							if (!iface.Get_ServiceType() == t)
								continue;

							Packet* request = new Packet(t, buffer, &bytes);
							srv->callback(t, srv, iface.Get_Id(), srv->Add_Client(iface.Get_Id(),t, remote, request), request);
							delete[] buffer;
							buffer = nullptr;

							delete request;
							request = nullptr;
							FD_CLR(iface.Get_Socket(), &copy_of_read);
							break;
						}

						if (FD_ISSET(iface.Get_Socket(), &copy_of_write))
						{
							FD_CLR(iface.Get_Socket(), &copy_of_write);
						}

						if (FD_ISSET(iface.Get_Socket(), &copy_of_except))
						{
							FD_CLR(iface.Get_Socket(), &copy_of_except);
						}
					}

					if (iFaceID != 0)
					{
						srv->Remove_Interface(iFaceID);
						iFaceID = 0;
					}
				}
			}
		}

		_USHORT Server::Get_Interface_by_Address(const std::string& mac, const ServiceType& type, const _IPADDR& address)
		{
			std::vector<Iface> _interfaces;

			for (Iface & iface : Get_Interfaces())
			{
				const _IPADDR ifaceIP = iface.Get_IPAddress();
				if (iface.Has_ARPEntry(mac) && memcmp(&address, &ifaceIP,
					sizeof(_IPADDR)) == 0 && iface.Get_ServiceType() == type)
				{
					return iface.Get_Id();
				}
			}

			return 0;
		}

		void Server::Send(const ServiceType& type, _USHORT iface, Client* client)
		{
			if (client->Get_ServiceType() != type)
				return;

			switch (type)
			{
			case BINL_SERVER:
			case DHCP_SERVER:
				switch (client->Get_DHCP_Client()->Get_State())
				{
				case DHCP_RELAY:
					this->Get_Interface(type, iface)->Send(client->Get_Relay_Hint(), client->response);
					break;
				case DHCP_SERVERRESPONSE:
					this->Get_Interface(type, iface)->Send(client->Get_Server_Hint(), client->response);
					break;
				case DHCP_CLIENTRESPONSE:
				default:
					this->Get_Interface(type, iface)->Send(client->Get_Client_Hint(), client->response);
					break;
				}

				break;
			case TFTP_SERVER:
			default:
				this->Get_Interface(type, iface)->Send(client->Get_Client_Hint(), client->response);
				break;
			}
		}

		Iface* Server::Get_Interface(const ServiceType& type, const _USHORT& id)
		{
			for (Iface & iface : Get_Interfaces())
			{
				if (iface.Get_ServiceType() == type && iface.Get_Id() == id)
					return &iface;
			}

			return nullptr;
		}

		bool Server::Listen(std::vector<std::thread>* threads)
		{
			threads->emplace_back(__Listen, global_srv_ptr);
			return threads->size() != 0;
		}

		void Server::HeartBeat()
		{
			for (Iface& iface : Get_Interfaces())
			{
				bool isListening = iface.Heartbeat();

				if (!isListening)
					iface.Close();
			}

			for (const std::pair<std::string,Client*>& client : this->clients)
				client.second->HeartBeat();
		}

		bool Server::Close()
		{
			__ClearSocketList(this);

			for (Iface& iface : Get_Interfaces())
				iface.Close();

			if (this->Interfaces.size() != 0)
				this->Interfaces.clear();

			return true;
		}

		void Server::Remove_Interface(const _USHORT& id)
		{
			this->Interfaces.erase(this->Interfaces.begin() + this->_GetInterfacePosition(id));
		}

		bool Server::Has_Client(const std::string& key)
		{
			return clients.find(key) != clients.end();
		}

		Client* Server::Add_Client(const _USHORT& iface, const ServiceType& t, const sockaddr_in& remote, Packet* packet)
		{
			std::string _ip = Functions::AddressStr(remote.sin_addr.s_addr, remote.sin_family);
			std::string _id = _ip;

			switch (t)
			{
			case DHCP_SERVER:
			case BINL_SERVER:
				_id = packet->get_hwaddress();
				break;
			default:
				_id = _ip;
				break;
			}

			if (Has_Client(_id))
			{
				clients.at(_id)->Set_Port(remote.sin_port);

				if (remote.sin_port != 67)
					clients.at(_id)->Set_Client_Hint(remote);
				else
					clients.at(_id)->Set_Server_Hint(remote);

				clients.at(_id)->Set_ServiceType(t);

				return clients.at(_id);
			}

			clients.emplace(_id, new Client(t, _ip, remote, _id));
			clients.at(_id)->SetIncomingInterface(iface);

			return clients.at(_id);
		}

		const _INT32 Server::_GetInterfacePosition(const _USHORT& id)
		{
			_INT32 pos = 0;

			for (_SIZET i = 0; i < this->Interfaces.size(); i++)
			{
				if (this->Interfaces.at(i).Get_Id() == id)
				{
					pos = static_cast<_INT32>(i);
					break;
				}
			}

			return pos;
		}

		_INT32 Server::Get_Num_Interfaces(const ServiceType& type)
		{
			_INT32 count = 0;
			
			for (Iface& iF : this->Interfaces)
				if (iF.Get_ServiceType() == type)
					count += 1;

			return count;
		}

		std::vector<Iface>& Server::Get_Interfaces()
		{
			return this->Interfaces;
		}

		void Server::Get_Interfaces(const ServiceType& type, std::vector<Iface>* interfaces)
		{
			for (Iface& iF : this->Interfaces)
				if (iF.Get_ServiceType() == type)
					interfaces->emplace_back(iF);
		}

		bool Server::Is_Running()
		{
			return this->started;
		}

		fd_set & Server::Get_Read_Descriptors()
		{
			return this->master_read;
		}

		fd_set & Server::Get_Write_Descriptors()
		{
			return this->master_write;
		}
		
		fd_set & Server::Get_Except_Descriptors()
		{
			return master_except;
		}
	}
}
