#pragma once
#include <Namiono/Namiono.h>
namespace Namiono
{
	namespace Services
	{
		class ProxyDHCP_Service :
			public Service
		{
		public:
			virtual void Handle_Service_Request(const ServiceType& type, Namiono::Network::Server* server, int iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet);

			void Handle_Request_Request(const ServiceType& type, Namiono::Network::Server* server, int iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet);

			ProxyDHCP_Service();
			~ProxyDHCP_Service();
		private:

			std::string _serverName = "";
			_IPADDR _bootServer = 0;
		};
	}
}
