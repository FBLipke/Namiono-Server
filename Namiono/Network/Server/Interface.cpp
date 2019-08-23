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
		Iface::Iface()
		{
		}

		Iface::Iface(const _IPADDR address, const _USHORT id, const _USHORT port)
		{
			this->_address = address;
			this->_port = port;
			this->_id = id;
		}

		Iface::~Iface()
		{
		}

		bool Iface::Init()
		{
			this->_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

			ClearBuffer(&this->_local, sizeof this->_local);
			this->_local.sin_addr.s_addr = this->_address;
			this->_local.sin_family = AF_INET;
			this->_local.sin_port = BS16(this->_port);

			_INT32 yes = 1;
			
			_INT32 retval = SOCKET_ERROR;
			_INT32 val_length = sizeof(_INT32);

			retval = setsockopt(this->_socket, SOL_SOCKET, SO_BROADCAST, (char*)&yes, val_length);
			retval = setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, val_length);

			return true;
		}

		std::string Iface::Get_ServerName() const
		{
			return Get_Hostname();
		}

		bool Iface::Start()
		{

			int retval = bind(this->_socket, reinterpret_cast<struct sockaddr*>(&this->_local), sizeof(this->_local));
			if (retval == SOCKET_ERROR)
			{
				printf("[E] Error (bind): Cannot bind to Interface %ud\n", this->Get_Id());
				return false;
			}

			printf("[I] Listening on %s:%d\n", Functions::AddressStr(this->Get_IPAddress()).c_str(), this->Get_Port());

			return  retval == 0;
		}

		void Iface::Send(sockaddr_in& hint, Packet* response)
		{
			int retval = sendto(this->Get_Socket(), response->Get_Buffer(), static_cast<int>(response->get_Length()),
				0, reinterpret_cast<struct sockaddr*>(&hint), sizeof hint);
		}

		bool Iface::Close()
		{
			_close(this->Get_Socket());

			return true;
		}
		
		bool Iface::Heartbeat()
		{
			return false;
		}

		std::vector<std::string> Iface::Get_ARPCache()
		{
			return this->arpCache;
		}

		void Iface::Add_ARPEntry(const std::string& mac)
		{
#ifdef _DEBUG
			printf("[D] Added ARP Entry for %s on Interface %d\n", mac.c_str(), Get_Id());
#endif
			this->arpCache.emplace_back(mac);
		}

		bool Iface::Has_ARPEntry(const std::string & mac)
		{
			for (size_t i = 0; i < arpCache.size(); i++)
			{
				if (arpCache.at(i) == mac)
					return true;
			}

			return false;
		}

		_USHORT Iface::Get_Id() const
		{
			return this->_id;
		}

		_USHORT Iface::Get_Port() const
		{
			return this->_port;
		}

		_IPADDR Iface::Get_IPAddress() const
		{
			return this->_address;
		}

		_SOCKET& Iface::Get_Socket()
		{
			return this->_socket;
		}
	}
}
