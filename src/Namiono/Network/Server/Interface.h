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
		class Iface
		{
		public:
			Iface();
			Iface(SETTINGS* settings, const ServiceType& type, const std::string& name, const int index, const _IPADDR address,
				const _IPADDR netmask, const _IPADDR gateway, const _USHORT id, const _USHORT port);

			virtual ~Iface();

			bool Init();
			bool Start();
			void Send(sockaddr_in& hint, Packet* response);
			bool Close();
			bool Heartbeat();
			std::vector<std::string> Get_ARPCache();

			void Add_ARPEntry(const std::string& mac);
			bool Has_ARPEntry(const std::string& mac);

			_USHORT Get_Id() const;
			_USHORT Get_Port() const;

			_IPADDR Get_IPAddress();
			_IPADDR Get_Gateway() const;
			_IPADDR Get_Netmask() const;

			bool IsUpstreamInterface() const;

			std::string Get_ServerName() const;
			_IPADDR Get_MulticastIP() const;
			_SOCKET& Get_Socket();
			ServiceType& Get_ServiceType();
		private:
			_IPADDR _address;
			_IPADDR _netmask;
			_IPADDR _gateway;
			std::string ifname;

			_USHORT _id;
			_USHORT _port;

			bool isUpstreamInterface = false;
			std::vector<std::string> arpCache;

			sockaddr_in _local;
			_INT32 local_length;
			ServerMode Servermode;
			ServiceType serviceType;
			SETTINGS* settings;

			_SOCKET _socket;
			ip_mreq mreq;
#ifndef _WIN32
			struct ifreq ifr;
#endif
		};
	}
}
