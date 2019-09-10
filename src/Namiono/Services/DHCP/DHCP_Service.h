#pragma once

#ifndef DHCP_SERVICE_H
#define DHCP_SERVICE_H

#include <Namiono/Namiono.h>
using namespace Namiono::Network;

namespace Namiono
{
	namespace Services
	{
		class DHCP_Service : public Service {
		public:
			DHCP_Service(SETTINGS* settings, const std::vector<_IPADDR>& upstreamServers);

			virtual void Handle_Service_Request(const ServiceType& type, Namiono::Network::Server* server, _INT32 iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet);

			void Handle_DHCP_Request(const ServiceType& type, Namiono::Network::Server* server, _INT32 iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet);

			void Handle_DHCP_Response(const ServiceType& type, Namiono::Network::Server* server, _INT32 iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet);

			~DHCP_Service();
		private:
			std::vector<_IPADDR> upstreamServers;
			std::map<std::string, DHCP_RELAYSESSION> relaySessions;

			SETTINGS* settings;
		};
	}
}
#endif /* DHCP_SERVICE_H */

