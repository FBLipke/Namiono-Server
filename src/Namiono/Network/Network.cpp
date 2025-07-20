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
		void Handle_Request(ServiceType type, Server* server, _USHORT iface, Client* client, Packet* packet)
		{
			services.at(client->Get_ServiceType()).get()->
				Handle_Service_Request(type, server, iface, client, packet);
		}

#ifdef _WIN32
		bool Network::Init_Winsock(_INT32 major, _INT32 minor)
		{
			WSADATA wsa;
			ClearBuffer(&wsa, sizeof wsa);

			return WSAStartup(MAKEWORD(major, minor), &wsa) == 0;
		}

		bool Network::Close_Winsock()
		{
			return WSACleanup() == 0;
		}
#endif
#ifdef ENABLE_UPSTREAMSERVER
		void Network::ReadServerList()
		{
			const std::string _serverlistFile = Combine(this->settings->CONFDIR, "servers.txt");

			FILE* fil = fopen(_serverlistFile.c_str(), "r");

			if (fil == nullptr)
			{
				printf("[E] File \"%s\" not found!\n", _serverlistFile.c_str());
				return;
			}

			char line[1024];
			ClearBuffer(line, sizeof line);
			while (fgets(line, sizeof line, fil) != nullptr)
			{
				char desc[64];
				ClearBuffer(desc, sizeof desc);
				char addr[1024];
				ClearBuffer(addr, sizeof addr);

				std::vector<_IPADDR> addrs;

				if (sscanf(line, "%s | %s", &addr, &desc) != 0)
				{
					const std::string _addrline = std::string(addr);
					std::vector<std::string> _addrs = Functions::Split(_addrline, std::string(","));

					for (_SIZET i = 0; i < _addrs.size(); i++)
						addrs.emplace_back(inet_addr(_addrs.at(i).c_str()));
				}

				for (_SIZET i = 0; i < addrs.size(); i++)
					Get_UpstreamServers()->emplace_back(DHCP_UPSTREAMSERVER(addrs.at(i), 67));
			}

			fclose(fil);
			fil = nullptr;
		}
#endif
		Network::Network(SETTINGS* settings, const std::string& rootDir)
		{
#ifdef _WIN32
			if (!Init_Winsock(2, 2))
			{
				printf("[E] Error: %s\n", strerror(WSAGetLastError()));
				return;
			}
#endif
			this->settings = settings;

			using namespace Namiono::Services;
#ifdef ENABLE_UPSTREAMSERVER
			this->ReadServerList();

			services.emplace(DHCP_SERVER, std::make_shared<DHCP_Service>(this->settings, *Get_UpstreamServers()));
#else
			services.emplace(DHCP_SERVER, std::make_shared<DHCP_Service>(this->settings));
#endif
			services.emplace(BINL_SERVER, std::make_shared<ProxyDHCP_Service>(this->settings));
			services.emplace(TFTP_SERVER, std::make_shared<TFTP_Service>(this->settings, rootDir));

			servers.emplace_back(settings, Handle_Request);
		}


		Network::~Network()
		{
#ifdef _WIN32
			if (!Close_Winsock())
			{
				printf("[E] Closing Winsock - Error: %d\n", WSAGetLastError());
				return;
			}
#endif
		}

		void Network::Init()
		{
			for (_SIZET i = 0; i < servers.size(); i++)
				servers.at(i).Init();
		}

		void Network::Start()
		{
			printf("[I] Starting network...\n");

			for (_SIZET i = 0; i < servers.size(); i++)
				servers.at(i).Start();
		}

		void Network::HeartBeat()
		{
			for (_SIZET i = 0; i < servers.size(); i++)
				servers.at(i).HeartBeat();
		}

		void Network::Listen()
		{
			for (_SIZET i = 0; i < servers.size(); i++)
				servers.at(i).Listen(&listenThreads);

			for (_SIZET i = 0; i < listenThreads.size(); i++)
				listenThreads.at(i).join();
		}

		std::vector<BootServerEntry>* Network::Get_BootServers()
		{
			return &serverlist;
		}

#ifdef ENABLE_UPSTREAMSERVER
		std::vector<DHCP_UPSTREAMSERVER>* Network::Get_UpstreamServers()
		{
			return &dhcpservers;
		}
#endif
		void Network::Close()
		{
			printf("[I] Closing network...\n");

			for (_SIZET i = 0; i < servers.size(); i++)
				servers.at(i).Close();
		}
	}
}
