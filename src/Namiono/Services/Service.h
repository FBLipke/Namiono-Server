#pragma once

#include <Namiono/Namiono.h>

#ifndef SERVICE_H
#define SERVICE_H

namespace Namiono
{
	namespace Services
	{
		class Service
		{
		public:
			Service() {};

			virtual void Start() = 0;
			virtual void Heartbeart() = 0;
			virtual void Init() = 0;
			virtual void Close() = 0;

			virtual void Handle_Service_Request(const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet) = 0;

			virtual ~Service() {};
		private:

		};
	}
}
#endif /* SERVICE_H */

