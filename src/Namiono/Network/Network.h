
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
		static std::map<ServiceType, std::shared_ptr<Namiono::Services::Service>> services;
		static std::vector<BootServerEntry> serverlist;
#ifdef ENABLE_UPSTREAMSERVER
		static std::vector<DHCP_UPSTREAMSERVER> dhcpservers;
#endif
		class Network
		{
		public:
			Network(SETTINGS* settings, const std::string& rootDir);
			~Network();

			void Init();

			void Start();
			void HeartBeat();
			void Listen();

			static std::vector<BootServerEntry>* Get_BootServers();
#ifdef ENABLE_UPSTREAMSERVER
			static std::vector<DHCP_UPSTREAMSERVER>* Get_UpstreamServers();
#endif
#ifdef _WIN32
			bool Init_Winsock(_INT32 major, _INT32 minor);
			bool Close_Winsock();
#endif
			void Close();
		private:
			_ULONG requestId = 0;
			_USHORT id = 1;
			std::vector<Server> servers;
			std::vector<std::thread>listenThreads;
			SETTINGS* settings;
#ifdef ENABLE_UPSTREAMSERVER
			void ReadServerList();
#endif
		};
	}
}
