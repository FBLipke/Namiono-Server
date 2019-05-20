/*
 * Server.h
 *
 *  Created on: 18.02.2019
 *      Author: lipkegu
 */

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
			Server(ServiceType* type, int family, _IPADDR address, _USHORT port, const std::string& rootDir,
				std::function<void(ServiceType*, Server<S>*, Client<S>*, std::string&, Packet*)> cb)
			{
				this->type = *type;
				this->_address = address;
				this->_port = port;
				this->family = family;
				this->callback = cb;

				this->rootDir = rootDir;
				this->listener.sin_addr.s_addr = INADDR_ANY;
				this->listener.sin_family = this->family;
				this->listener.sin_port = htons(this->_port);
				this->_socket = socket(this->family, SOCK_DGRAM, IPPROTO_UDP);

				Set_Option(SOL_SOCKET, SO_REUSEADDR, 1);
				Set_Option(SOL_SOCKET, SO_BROADCAST, 1);
				Set_Option(SOL_SOCKET, SO_REUSE_UNICASTPORT, 1);
			}

			~Server()
			{
				_close(this->_socket);
			}

			bool Set_Option(_USHORT level, _USHORT option, int value)
			{
				return setsockopt(this->_socket, level,
					option, reinterpret_cast<const char*>(&value), sizeof value) == 0;
			}

			Client<S>* Add_Client(const S& remote)
			{
				std::string addr = Functions::AddressStr(
					remote.sin_addr.s_addr, remote.sin_family);

				if (clients.find(addr) != clients.end())
				{
					clients.at(addr)->Set_Port(remote.sin_port);
					
					return clients.at(addr).get();
				}

				clients.emplace(addr, new Client<S>(type, Functions::AddressStr(this->Get_IPAddress()), remote, addr));

				return clients.at(addr).get();
			}

			_IPADDR Get_IPAddress() const
			{
				return this->_address;
			}

			std::string Get_ServerName() const
			{
				return Get_Hostname();
			}

			void Remove_Client(Client<S>* client)
			{
				if (clients.find(client->Get_ID()) != clients.end())
				{
					printf("[I] Dropping %s Client \"%s\"\n", 
						client->Get_TypeString().c_str(),
							client->Get_ID().c_str());
					clients.erase(client->Get_ID());
				}
			}

			int Listen()
			{
#ifdef _WIN32
				int listener_length = sizeof this->listener;
#else
				socklen_t listener_length = sizeof this->listener;
#endif
				int retval = bind(this->_socket, reinterpret_cast<const struct sockaddr*>(&this->listener), listener_length);
				if (retval == SOCKET_ERROR)
				{
					printf("[E] bind(): %s (Address: %s)\n", strerror(errno),
						inet_ntoa(this->listener.sin_addr));

					return retval;
				}

				if (retval == -1)
				{
					printf("[E] bind(): %s\n", strerror(errno));
					return retval;
				}

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
					if (bytes == -1)
					{
						printf("[E] recvfrom(): %s\n", strerror(errno));
						continue;
					}

					Packet * request = new Packet(type, buffer, &bytes);
					delete[] buffer;

					callback(&type, this, Add_Client(remote), this->rootDir, request);
				
					delete request;
				}

				return retval;
			}

			int Send(Client<S>* client, Packet* packet)
			{
				S _hint = client->Get_Hint();

				return sendto(this->_socket, packet->Get_Buffer(), static_cast
					<int>(packet->get_Length()), 0, reinterpret_cast<struct sockaddr*>(&_hint), sizeof _hint);
			}

		private:
			std::function<void(ServiceType*, Server<S>*, Client<S>*, std::string&, Packet*)> callback;
			std::map<std::string, std::unique_ptr<Client<S>>> clients;
			_IPADDR _address = 0;
			_USHORT _port;
			S listener;
			_SOCKET _socket;
			int family;
			ServiceType type;
			std::string hname;
			std::string ident;
			std::string rootDir;
		};

		template<class S>
		void __serverThread(ServiceType type, int family, _IPADDR address, _USHORT port, const std::string& rootDir,
				std::function<void(ServiceType*, Server<S>*, Client<S>*, std::string&, Packet*)> cb)
		{
			Server<S>* serv = new Server<S>(&type, family, address, port, rootDir, cb);
			serv->Listen();
		}

	}
}

#endif
