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
			DHCP_Service(SETTINGS* settings, const std::vector<DHCP_UPSTREAMSERVER>& upstreamServers);

			virtual void Handle_Service_Request(const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet);

			void Handle_DHCP_Request(const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet);

			void Handle_DHCP_Response(const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet);
			
			virtual void Start() override;
			virtual void Close() override;
			virtual void Heartbeart() override;
			virtual void Init() override;
			
			~DHCP_Service();
		private:
			std::vector<DHCP_UPSTREAMSERVER> upstreamServers;
			std::map<std::string, DHCP_RELAYSESSION> relaySessions;

			SETTINGS* settings;

			// Geerbt über Service

		};
	}
}
#endif /* DHCP_SERVICE_H */

