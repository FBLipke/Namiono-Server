/*
 * Network.h
 *
 *  Created on: 18.02.2019
 *      Author: lipkegu
 */

#ifndef NAMIONO_NETWORK_NETWORK_H_
#define NAMIONO_NETWORK_NETWORK_H_

#include <Namiono/Network/Packet/Packet.h>
#include <Namiono/Network/Client/Client.h>
#include <Namiono/Network/Server/Server.h>

namespace Namiono
{
	namespace Network
	{
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
		void Handle_WDS_Options(Server<S>* server, Client<S>* client, Packet* response);
		template<class S>
		void Handle_DHCP_Discover(ServiceType* type, Server<S>* server, Client<S>* client, Packet* packet);

		void Create_BootServer_List(std::vector<DHCP_Option>& vendorOps);
		template<class S>
		void GenerateBootMenue(Client<S>* client, std::vector<DHCP_Option>* vendorOpts);
		template<class S>
		std::string Get_Bootfile_Path(Client<S>* client);
		template<class S>
		void Handle_IPXE_Options(Server<S>* server, Client<S>* client, Packet* response);

	}
}

#endif /* NAMIONO_NETWORK_NETWORK_H_ */
