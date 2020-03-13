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
			virtual void Handle_Service_Request(const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet);

			void Handle_Request_Request(const SETTINGS* settings, const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet);

			virtual void Start() override;
			virtual void Close() override;
			virtual void Heartbeart() override;
			virtual void Init() override;

			ProxyDHCP_Service(SETTINGS* settings);
			~ProxyDHCP_Service();
		private:
			SETTINGS* settings = nullptr;

			// Geerbt über Service

		};
	}
}
