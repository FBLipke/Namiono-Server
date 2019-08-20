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

#pragma once

#ifndef NAMIONO_NETWORK_NETWORK_H_
#define NAMIONO_NETWORK_NETWORK_H_

#include <Namiono/Network/Packet/Packet.h>
#include <Namiono/Network/Client/Client.h>
#include <Namiono/Network/Server/Server.h>


namespace Namiono
{
	namespace Network
	{
#ifdef _WIN32
		bool Init_Winsock(_INT32 major, _INT32 minor);
		bool Close_Winsock();
#endif
		void Bootstrap_Network(const std::string& rootDir);
		template<class S>
		void Handle_Request(ServiceType* type, Server<S>* server, Client<S>* client, const std::string& rootDir, Packet* packet);
		template<class S>
		void Handle_DHCP_Request(ServiceType* type, Server<S>* server, Client<S>* client, Packet* packet);
		template<class S>
		void Handle_TFTP_RRQ(ServiceType* type, Server<S>* server, Client<S>* client, const std::string& rootDir, Packet* packet);
		template<class S>
		void Handle_TFTP_ACK(ServiceType* type, Server<S>* server, Client<S>* client, Packet* packet);
		template<class S>
		void Handle_TFTP_ERR(ServiceType* type, Server<S>* server, Client<S>* client, Packet* packet);
		template<class S>
		void Handle_WDS_Options(Server<S> * server, Client<S> * client);
		template<class S>
		void Handle_DHCP_Discover(ServiceType* type, Server<S>* server, Client<S>* client, Packet* packet);
		template<class S>
		void Create_BootServer_List(Client<S>* client);
		template<class S>
		void GenerateBootMenue(Client<S>* client);
		template<class S>
		void Handle_IPXE_Options(Server<S>* server, Client<S>* client, Packet* response);
		template<class S>
		void Handle_RIS_RQU(ServiceType * type, Server<S> * server, Client<S> * client, Packet * packet, const std::string& rootDir);
		template<class S>
		void Handle_RIS_NEG(ServiceType * type, Server<S> * server, Client<S> * client, Packet * packet);
		template<class S>
		void Handle_RIS_AUT(ServiceType * type, Server<S> * server, Client<S> * client, Packet * packet);

		template<class S>
		void Relay_Request_Packet(ServiceType * type, Server<S> * server, Client<S> * client, Packet * packet);
		template<class S>
		void Relay_Response_Packet(ServiceType * type, Server<S> * server, Client<S> * client, Packet * packet);
	}
}

#endif /* NAMIONO_NETWORK_NETWORK_H_ */
