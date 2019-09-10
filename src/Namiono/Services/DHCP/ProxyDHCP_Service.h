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
			virtual void Handle_Service_Request(const ServiceType& type, Namiono::Network::Server* server, _INT32 iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet);

			void Handle_Request_Request(const SETTINGS* settings, const ServiceType& type, Namiono::Network::Server* server, _INT32 iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet);

			ProxyDHCP_Service(SETTINGS* settings);
			~ProxyDHCP_Service();
		private:
			SETTINGS* settings = nullptr;
		};
	}
}
