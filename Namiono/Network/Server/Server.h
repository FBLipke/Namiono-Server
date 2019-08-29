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
#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Network
	{
#ifndef _WIn32
		int GetDefaultGw(_IPADDR& gw);
#endif

		class Server
		{
		public:
			Server();
			Server(std::vector<_IPADDR>* addrList, std::function<void(ServiceType, Server*, int, Client*, Packet*)> cb);

			virtual ~Server();

			bool Init();
			void Remove_Client(const std::string & ident);
			bool Start();

			int Get_Interface_by_Address(const _IPADDR & address);

			int Get_Interface_by_Mac(const std::string& mac, const _IPADDR& relayIP);

			void Send(int iface, Client* client);

			Iface * Get_Interface(const int & id);

			bool Listen(std::vector<std::thread>* threads);

			bool Close();

			bool Has_Client(const std::string & key);

			Client * Add_Client(const ServiceType& t, const sockaddr_in & remote);

			std::vector<Iface>& Get_Interfaces();

			bool Is_Running();
			fd_set& Get_Read_Descriptors();

			std::function<void(ServiceType, Server*, int, Client*, Packet*)> callback;
		private:
			bool started = false;
			Server* global_srv_ptr = nullptr;
			fd_set master_read;
			std::vector<Iface> Interfaces;
			std::map<std::string, Client*> clients;

		};
	}
}
