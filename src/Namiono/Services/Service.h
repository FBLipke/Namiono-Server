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
			virtual void Handle_Service_Request(const ServiceType& type, Namiono::Network::Server* server, _INT32 iface,
				Namiono::Network::Client* client, Namiono::Network::Packet* packet) = 0;

			virtual ~Service() {};
		private:

		};
	}
}
#endif /* SERVICE_H */

