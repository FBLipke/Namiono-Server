#pragma once

#ifndef TFTP_SERVICE_H
#define TFTP_SERVICE_H

#include <Namiono/Namiono.h>

using namespace Namiono::Network;

namespace Namiono
{
	namespace Services
	{
		class TFTP_Service :
			public Service
		{
		public:
			TFTP_Service(SETTINGS* settings, const std::string& rootDir);

			virtual void Handle_Service_Request(const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet);

			void Handle_RRQ_Request(const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet);

			void Handle_ACK_Request(const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet);

			void Handle_ERR_Request(const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet);

			virtual void Start() override;
			virtual void Close() override;
			virtual void Heartbeart() override;
			virtual void Init() override;

			~TFTP_Service();
		private:
			std::string _rootDir;
			SETTINGS* settings = nullptr;

			// Geerbt über Service

		};
	}
}
#endif