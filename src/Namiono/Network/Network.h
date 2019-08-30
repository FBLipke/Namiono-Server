#pragma once

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
		static std::map<ServiceType, Namiono::Services::Service*> services;
		static std::vector<BootServerEntry> serverlist;
		static std::vector<_IPADDR> dhcpservers;

		class Network
		{
		public:
			Network(const std::string& rootDir);
			~Network();

			void Init();

			void Start();

			void Listen();

			static std::vector<BootServerEntry>* Get_BootServers();
			static std::vector<_IPADDR>* Get_UpstreamServers();

#ifdef _WIN32
			bool Init_Winsock(_INT32 major, _INT32 minor);
			bool Close_Winsock();
#endif

			void Close();
		private:

			_ULONG requestId = 0;
			_USHORT id = 1;
			std::vector<Server> servers;
			std::vector<_IPADDR> addresses;
			std::vector<std::thread>listenThreads;

		};
	}
}
