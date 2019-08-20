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
#pragma once

#ifndef NAMIONO_NETWORK_SERVER_SERVER_H_
#define NAMIONO_NETWORK_SERVER_SERVER_H_

#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Network
	{
		template<class S>
		class Server
		{
		public:
			Server(ServiceType* type, _INT32 family, bool mcast_mode, _IPADDR address, _IPADDR gateway, _IPADDR netmask, _USHORT port, const std::string& rootDir,
				std::function<void(ServiceType*, Server<S>*, Client<S>*, std::string&, Packet*)> cb)
			{
				this->type = *type;
				this->_address = address;
				this->_gateway = gateway;
				this->_netmask = netmask;
				this->mcast_mode = mcast_mode;
				this->_port = port;
				this->family = family;
				this->callback = cb;
				this->clients = new std::map<std::string, Client<S>*>();
				this->rootDir = rootDir;
				this->listener.sin_addr.s_addr = mcast_mode ? inet_addr("224.0.2.1") : this->_address;
				this->listener.sin_family = this->family;
				this->listener.sin_port = htons(this->_port);
				this->_socket = socket(this->family, SOCK_DGRAM, IPPROTO_UDP);

				this->localInterface.s_addr = inet_addr(SETTINGS.DISCOVERY_ADDR.c_str());
				
				Set_Option(SOL_SOCKET, SO_REUSEADDR, SETTINGS.SOCKET_REUSEADDR);
				Set_Option(SOL_SOCKET, SO_BROADCAST, SETTINGS.SOCKET_BROADCAST);
				Set_Option(SOL_SOCKET, SO_REUSEPORT, SETTINGS.SOCKET_REUSEPORT);

				if (this->mcast_mode)
				{
					ClearBuffer(&mreq, sizeof mreq);
					mreq.imr_multiaddr.s_addr = inet_addr("224.0.2.1");
					mreq.imr_interface.s_addr = this->mcast_mode ? this->localInterface.s_addr : this->_address;

					setsockopt(this->_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
					setsockopt(this->_socket, IPPROTO_IP, IP_MULTICAST_IF, (char *)&this->localInterface, sizeof(this->localInterface));
				}
			}

			void Close()
			{
				for (const std::pair<const std::string, Client<S>*>& client : *this->clients)
					this->Remove_Client(client.first);

				this->clients->clear();

				delete this->clients;
				this->clients = nullptr;
				if (this->mcast_mode)
					setsockopt(this->_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));


				_close(this->_socket);
			}
			
			~Server()
			{
				this->Close();
			}

			bool Has_Client(const std::string& key)
			{
				return clients->find(key) != clients->end();
			}

			bool Set_Option(const _USHORT& level, const _USHORT& option, const char* value)
			{
				return setsockopt(this->_socket, level,
					option, reinterpret_cast<const char*>(&value), sizeof value) == 0;
			}

			bool Set_Option(const _USHORT& level, const _USHORT& option, const _INT32 value)
			{
				return setsockopt(this->_socket, level,
					option, reinterpret_cast<const char*>(&value), sizeof value) == 0;
			}

			Client<S>* Add_Client(const S& remote)
			{
				std::string addr = Functions::AddressStr(
					remote.sin_addr.s_addr, remote.sin_family);

				if (Has_Client(addr))
				{
					clients->at(addr)->Set_Port(remote.sin_port);
					return clients->at(addr);
				}

				clients->emplace(addr, new Client<S>(type, Functions::AddressStr(this->Get_IPAddress()), remote, addr));

				return clients->at(addr);
			}

			const _IPADDR& Get_IPAddress() const
			{
				return this->_address;
			}

			const _IPADDR& Get_NetMask() const
			{
				return this->_netmask;
			}

			const _IPADDR& Get_Gateway() const
			{
				return this->_gateway;
			}

			const _USHORT& Get_Port() const
			{
				return this->_port;
			}

			std::string Get_ServerName() const
			{
				return Get_Hostname();
			}

			void Remove_Client(const std::string& ident)
			{
				std::string _id = ident;

				if (Has_Client(_id))
				{
					delete clients->at(_id);
					clients->at(_id) = nullptr;

					clients->erase(_id);
				}
			}

			_INT32 Listen()
			{
#ifdef _WIN32
				_INT32 listener_length = sizeof this->listener;
#else
				socklen_t listener_length = sizeof this->listener;
#endif
				int retval = bind(this->_socket, reinterpret_cast<const struct sockaddr*>(&this->listener), listener_length);
				if (retval == SOCKET_ERROR)
					return retval;
				
				std::string _typestr = "";

				switch (this->type)
				{
				case DHCP_SERVER:
					_typestr = "DHCP (Proxy)";
					break;
				case BINL_SERVER:
					_typestr = "BINL";
					break;
				case TFTP_SERVER:
					_typestr = "TFTP";
					break;
				default:
					break;
				}

				printf("[I] %s Listening on %s:%d\n", _typestr.c_str(), Functions::AddressStr(this->Get_IPAddress()).c_str(), this->Get_Port());

				while (retval != SOCKET_ERROR)
				{
					socklen_t remote_len = 0;
					S remote;
					ClearBuffer(&remote, sizeof remote);

					char* buffer = new char[16385];
					ClearBuffer(buffer, 16385);

					_SIZET bytes = SOCKET_ERROR;
					remote_len = sizeof remote;
					bytes = recvfrom(this->_socket, buffer, 16385, 0, reinterpret_cast<struct sockaddr*>(&remote), &remote_len);
					if (bytes == SOCKET_ERROR)
					{
						printf("[E] recvfrom(): %s\n", strerror(errno));
						continue;
					}

					Packet* request = new Packet(&type, buffer, &bytes);
					delete[] buffer;
					buffer = nullptr;

					callback(&type, this, Add_Client(remote), this->rootDir, request);

					delete request;
					request = nullptr;
				}

				return retval;
			}

			_INT32 Send(const Client<S>* client)
			{
				S _hint = client->Get_Client_Hint();

				switch (this->type)
				{
				case DHCP_SERVER:
					if (client->dhcp->Get_State() == DHCP_RELAY)
					{
						_hint = client->Get_Relay_Hint();
					}
					break;
				default:
					break;
				}
							   
				
				if (this->mcast_mode)
				{
					return sendto(this->_socket, client->response->Get_Buffer(), static_cast
						<_INT32>(client->response->get_Length()), 0, reinterpret_cast<struct sockaddr*>(&this->listener), sizeof this->listener);
				}

				return sendto(this->_socket, client->response->Get_Buffer(), static_cast
					<_INT32>(client->response->get_Length()), 0, reinterpret_cast<struct sockaddr*>(&_hint), sizeof _hint);
			}

		private:
			std::function<void(ServiceType*, Server<S>*, Client<S>*, std::string&, Packet*)> callback;
			std::map<std::string, Client<S>*>* clients = nullptr;
			_IPADDR _address = 0;
			_IPADDR _gateway = 0;
			_IPADDR _netmask = 0;

			_USHORT _port = 0;
			S listener;
			_SOCKET _socket = 0;
			int family = 0;
			ServiceType type;
			std::string hname = "";
			std::string ident = "";
			std::string rootDir = "";
			in_addr localInterface;
			bool mcast_mode = false;
			ip_mreq mreq;
		};

		template<class S>
		void __serverThread(ServiceType type, _INT32 family, bool mcast_mode, _IPADDR address, _IPADDR gateway, _IPADDR netmask, _USHORT port, const std::string& rootDir,
			std::function<void(ServiceType*, Server<S>*, Client<S>*, std::string&, Packet*)> cb)
		{
			Server<S>* serv = new Server<S>(&type, family, mcast_mode, address, gateway, netmask, port, rootDir, cb);
			serv->Listen();
			delete serv;
			serv = nullptr;
		}

	}
}

#endif
