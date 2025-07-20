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
		class Server
		{
		public:
			Server();
			explicit Server(SETTINGS* settings, std::function<void(ServiceType, Server*, _USHORT, Client*, Packet*)> cb);
			void Add_Interface(const std::string& name, const _USHORT& index, const _IPADDR& address, const _IPADDR& netmask, const _IPADDR& gateway = 0);

			virtual ~Server();
			bool Init();
			void Remove_Client(const std::string & ident);
			bool Start();
			_USHORT Get_Interface_by_Address(const std::string& mac, const ServiceType& type, const _IPADDR& address);
			void Send(const ServiceType& type, _USHORT iface, Client* client);
			Iface * Get_Interface(const ServiceType& type, const _USHORT& id);
			bool Listen(std::vector<std::thread>* threads);
			bool Close();
			void HeartBeat();

			void Remove_Interface(const _USHORT& id);
			bool Has_Client(const std::string & key);
			Client * Add_Client(const _USHORT& iface, const ServiceType& t, const sockaddr_in & remote, Packet* packet);
		
			std::vector<Iface>& Get_Interfaces();
			void Get_Interfaces(const ServiceType& type, std::vector<Iface>* interfaces);
			bool Is_Running();

			fd_set& Get_Read_Descriptors();
			fd_set& Get_Write_Descriptors();
			fd_set& Get_Except_Descriptors();
			
			void Add_Interfaces();

			std::function<void(ServiceType, Server*, _USHORT, Client*, Packet*)> callback;
			std::vector<_IPADDR> addresses;
			_INT32 Get_Num_Interfaces(const ServiceType& type);
		private:
			bool started = false;
			Server* global_srv_ptr = nullptr;
			fd_set master_read;
			fd_set master_write;
			fd_set master_except;

			SETTINGS* settings;
			const _INT32 _GetInterfacePosition(const _USHORT& id);

			std::vector<Iface> Interfaces;
			std::map<std::string, std::shared_ptr<Client>> clients;
		};
	}
}
