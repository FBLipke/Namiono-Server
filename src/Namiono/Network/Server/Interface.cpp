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

#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Network
	{
		Iface::Iface()
		{
		}

		Iface::Iface(const std::string& name, const int index, const _IPADDR address, const _IPADDR netmask, const _IPADDR gateway, const _USHORT id, const _USHORT port)
		{
			this->ifname = name;
			this->_address = address;
			this->_gateway = gateway;
			this->_netmask = netmask;
			this->isUpstreamInterface = this->_gateway != 0;
			this->_port = port;
			this->_id = static_cast<_USHORT>(index);

			switch (this->_port)
			{
			case 67:
			case 69:
			case 4011:
				Servermode = UDP;
				break;
			case 9000:
			default:
				Servermode = TCP;
				break;
			}
		}

		Iface::~Iface()
		{
		}

		bool Iface::Init()
		{
			ClearBuffer(&this->_local, sizeof this->_local);
			this->_local.sin_addr.s_addr = INADDR_ANY;

			switch (Servermode)
			{
			case TCP:
				this->_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				this->_local.sin_family = AF_INET;
				break;
			case TCP6:
				this->_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
				this->_local.sin_family = AF_INET6;
				break;
			case UDP:
				this->_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
				this->_local.sin_family = AF_INET;
				break;
			case UDP6:
				this->_socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
				this->_local.sin_family = AF_INET6;
				break;
			case UDPMCAST:
				this->_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
				this->_local.sin_family = AF_INET;
				break;
			case UNKNOWN:
			default:
				this->_socket = socket(AF_UNSPEC, SOCK_RAW, 0);
				this->_local.sin_family = AF_UNSPEC;
				break;
			}

			_INT32 retval = SOCKET_ERROR;
			this->_local.sin_port = htons(this->_port);

			_INT32 yes = 1;
			_INT32 val_length = sizeof(_INT32);

			retval = setsockopt(this->_socket, SOL_SOCKET, SO_BROADCAST, (char*)&yes, val_length);
			retval = setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, val_length);

			ClearBuffer(&mreq, sizeof mreq);
			mreq.imr_multiaddr.s_addr = inet_addr(SETTINGS.DISCOVERY_ADDR.c_str());
			mreq.imr_interface.s_addr = this->_local.sin_addr.s_addr;

			setsockopt(this->_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
			setsockopt(this->_socket, IPPROTO_IP, IP_MULTICAST_IF, (char *)&mreq.imr_interface.s_addr, sizeof(mreq.imr_interface.s_addr));
#ifndef _WIN32
			memset(&ifr, 0, sizeof(ifr));
			snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), this->ifname.c_str());
			retval = setsockopt(this->_socket, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr));

#endif
			return true;
		}

		std::string Iface::Get_ServerName() const
		{
			return Functions::Get_Hostname();
		}

		_IPADDR Iface::Get_MulticastIP() const
		{
			return mreq.imr_multiaddr.s_addr;
		}

		bool Iface::Start()
		{
			int retval = bind(this->_socket, reinterpret_cast<struct sockaddr*>(&this->_local), sizeof this->_local);
			if (retval == SOCKET_ERROR)
			{
				printf("[E] Error (bind): Cannot bind to Interface %u\n", this->Get_Id());
				return false;
			}

			return  retval == 0;
		}

		void Iface::Send(sockaddr_in& hint, Packet* response)
		{
			int retval = sendto(this->Get_Socket(), response->Get_Buffer(), static_cast<int>(response->get_Length()),
				0, reinterpret_cast<struct sockaddr*>(&hint), sizeof hint);

			if (retval == SOCKET_ERROR)
			{
				printf("[E] Error (sendto): Cannot send on Interface %u\n", this->Get_Id());
				return;
			}
		}

		bool Iface::Close()
		{
			setsockopt(this->_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
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
			this->arpCache.emplace_back(mac);
		}

		bool Iface::Has_ARPEntry(const std::string & mac)
		{
			for (_SIZET i = 0; i < arpCache.size(); i++)
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

		_IPADDR Iface::Get_IPAddress()
		{
			return this->_address;
		}

		_IPADDR Iface::Get_Netmask() const
		{
			return this->_netmask;
		}

		bool Iface::IsUpstreamInterface() const
		{
			return this->isUpstreamInterface;
		}

		_IPADDR Iface::Get_Gateway() const
		{
			return this->_gateway;
		}

		_SOCKET& Iface::Get_Socket()
		{
			return this->_socket;
		}
	}
}
