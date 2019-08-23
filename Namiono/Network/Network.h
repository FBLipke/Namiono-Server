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
#include <Namiono/Network/Server/Interface.h>
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
		void Handle_Request(ServiceType type, Server* server, int iface, Client* client, const std::string rootDir, Packet* packet);
		void Handle_DHCP_Request(ServiceType* type, Server* server, int iface, Client* client, Packet* packet);
		void Handle_TFTP_RRQ(ServiceType* type, Server* server, int iface, Client* client, const std::string& rootDir, Packet* packet);
		void Handle_TFTP_ACK(ServiceType* type, Server* server, int iface, Client* client, Packet* packet);
		void Handle_TFTP_ERR(ServiceType* type, Server* server, int iface, Client* client, Packet* packet);
		void Handle_WDS_Options(Server* server, int iface, Client* client);
		void Handle_DHCP_Discover(ServiceType* type, Server* server, int iface, Client* client, Packet* packet);
		void Create_BootServer_List(Client* client);
		void GenerateBootMenue(Client* client);
		void Handle_IPXE_Options(Server* server, int iface, Client* client, Packet* response);
		void Handle_RIS_RQU(ServiceType * type, Server* server, int iface, Client* client, Packet * packet, const std::string& rootDir);
		void Handle_RIS_NEG(ServiceType * type, Server* server, int iface, Client* client, Packet * packet);
		void Handle_RIS_AUT(ServiceType * type, Server* server, int iface, Client* client, Packet * packet);
		void Relay_Request_Packet(ServiceType * type, Server* server, int iface, Client* client, Packet * packet);
		void Relay_Response_Packet(ServiceType * type, Server* server, int iface, Client* client, Packet * packet);
	}
}

#endif /* NAMIONO_NETWORK_NETWORK_H_ */
