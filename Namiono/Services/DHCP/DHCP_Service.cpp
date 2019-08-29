/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

 /*
  * File:   DHCP_Service.cpp
  * Author: guido
  *
  * Created on 25. August 2019, 01:04
  */

#include <Namiono/Namiono.h>
namespace Namiono
{
	namespace Services
	{
		DHCP_Service::DHCP_Service(const std::vector<_IPADDR>& upstreamServers)
		{
			this->upstreamServers = upstreamServers;
		}

		void DHCP_Service::Handle_Service_Request(const ServiceType& type, Namiono::Network::Server* server, int iface,
			Namiono::Network::Client* client, Namiono::Network::Packet* packet)
		{
			Network::Network::Get_BootServers()->clear();
			std::vector<_IPADDR> addresses;


			if (packet->Has_DHCPOption(60))
			{
				if (Functions::Compare(packet->Get_DHCPOption(static_cast<_BYTE>(60)).Value, "PXEClient", 9))
					client->dhcp->Set_Vendor(PXEClient);

				if (Functions::Compare(packet->Get_DHCPOption(static_cast<_BYTE>(60)).Value, "PXEServer", 9))
					client->dhcp->Set_Vendor(PXEServer);

				if (Functions::Compare(packet->Get_DHCPOption(static_cast<_BYTE>(60)).Value, "AAPLBSDPC", 9))
				{
					client->dhcp->Set_Vendor(AAPLBSDPC);
					client->dhcp->SetIsBSDPRequest(true);
				}
			}

			switch (packet->get_opcode())
			{
			case BOOTREQUEST:
				if (!packet->Has_DHCPOption(53))
					return;
				printf("[I] DHCP : Request on %s...\n", Functions::AddressStr(
					server->Get_Interface(iface)->Get_IPAddress()).c_str());

				switch (static_cast<DHCP_MSGTYPE>(packet->Get_DHCPOption(53).Get_Value_As_Byte()))
				{
				case DISCOVER:
					this->Handle_Discover_Request(type, server, iface, client, packet);
					break;
				case INFORM:
				case REQUEST:
					this->Handle_Request_Request(type, server, iface, client, packet);
					break;
				default:
					server->Remove_Client(client->Get_ID());
					break;
				}
				break;
			case BOOTREPLY:

				if (!packet->Has_DHCPOption(53))
					return;

				printf("[I] DHCP : Response on %s...\n", Functions::AddressStr(
					server->Get_Interface(iface)->Get_IPAddress()).c_str());

				for (Iface& iface : server->Get_Interfaces())
					addresses.emplace_back(iface.Get_IPAddress());

				DHCP_Functions::Add_BootServer(Network::Network::Get_BootServers(), Functions::Get_Hostname(), addresses);
				DHCP_Functions::Add_BootServer_To_ServerList(Network::Network::Get_BootServers(), server, client, packet->get_servername());

				switch (static_cast<DHCP_MSGTYPE>(packet->Get_DHCPOption(53).Get_Value_As_Byte()))
				{
				case ACK:
				case OFFER:
					this->Handle_DHCP_Response(type, server, iface, client, packet);
					break;
				default:
					break;
				}
				break;
			}

			if (client->dhcp->Get_State() == CLIENTSTATE::DHCP_DONE ||
				client->dhcp->Get_State() == CLIENTSTATE::DHCP_ABORT)
				server->Remove_Client(client->Get_ID());

		}

		void DHCP_Service::Handle_Discover_Request(const ServiceType& type, Namiono::Network::Server* server, int iface,
			Namiono::Network::Client* client, Namiono::Network::Packet* packet)
		{
			if (upstreamServers.size() == 0)
			{
				printf("[W] DHCP : No upstreamservers available!\n");
				return;
			}

			client->response = new Packet(type, *packet, packet->get_Length());
			DHCP_Functions::Relay_Request_Packet(inet_addr("10.20.0.1"), 67, type, server, iface, client);

			/*switch (type)
			{
			case DHCP_SERVER:

				break;
			case BINL_SERVER:
				switch (client->dhcp->Get_Vendor())
				{
				case PXEClient:
					DHCP_Functions::Create_BootServerList(client);
					DHCP_Functions::Generate_Bootmenu_From_ServerList(client);

					if (SETTINGS.PXEBOOTMENUE == 1)
					{
						client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>
							(PXE_DISCOVERY_CONTROL), static_cast<_BYTE>(SETTINGS.DISCOVERY_MODE));

						client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_DISCOVERY_MCADDR),
							static_cast<_IPADDR>(inet_addr(SETTINGS.DISCOVERY_ADDR.c_str())));

						client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_MTFTP_IP_ADDR),
							static_cast<_IPADDR>(inet_addr(SETTINGS.DISCOVERY_ADDR.c_str())));

						client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_MTFTP_SERVER_PORT), SETTINGS.MTFTP_SPORT);
						client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_MTFTP_CLIENT_PORT), SETTINGS.MTFTP_CPORT);

						client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_DOMAIN), SETTINGS.NBDOMAIN);
						client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_SERVER), server->Get_Interface(iface)->Get_ServerName());
						client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_DISCOVERY), static_cast<_BYTE>(1));
						client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_CONFIGURED), static_cast<_BYTE>(1));
						client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_VERSION), BS32(static_cast<_UINT>(1)));
						client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_SERIALNO), std::string("Namiono - Server 0.5"));
					}

					client->response = new Packet(type, *packet, 1024, DHCP_MSGTYPE::OFFER);
					if (client->dhcp->vendorOpts->size() != 0)
						client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(43),
							*client->dhcp->vendorOpts));

					client->response->set_servername(server->Get_Interface(iface)->Get_ServerName());
					client->response->set_nextIP(server->Get_Interface(iface)->Get_IPAddress());


					client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(1), static_cast<_ULONG>(server->Get_Interface(iface)->Get_Netmask())));
					client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(53), static_cast<_BYTE>(OFFER)));
					client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(54), static_cast<_ULONG>(client->dhcp->GetNextServer())));
					client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(60), client->dhcp->Get_VendorString()));

					DHCP_Functions::Handle_WDS_Options(server, iface, client);
					break;
				default:
					return;
				}

				client->response->set_filename(client->dhcp->GetBootfile());
				client->response->Commit();

				// Sometimes we responding too fast, so wait here... x ms;
#ifdef _WIN32
				Sleep(static_cast<_BYTE>((1 + SETTINGS.SUBNETDELAY)));
#else
				sleep(static_cast<_BYTE>((1 + SETTINGS.SUBNETDELAY)));
#endif

				server->Send(iface, client);

				delete client->response;
				client->response = nullptr;

				client->dhcp->Set_State(DHCP_DONE);
			default:
				break;
			}
			*/

			delete client->response;
			client->response = nullptr;
		}


		void DHCP_Service::Handle_Request_Request(const ServiceType& type, Namiono::Network::Server* server, int iface,
			Namiono::Network::Client* client, Namiono::Network::Packet* packet)
		{
			if (upstreamServers.size() == 0)
			{
				printf("[W] DHCP : No upstreamservers available!\n");
				return;
			}

			client->response = new Packet(type, *packet, packet->get_Length());
			DHCP_Functions::Relay_Request_Packet(inet_addr("10.20.0.1"), 67, type, server, iface, client);

			
			delete client->response;
			client->response = nullptr;
		}

		void DHCP_Service::Handle_DHCP_Response(const ServiceType& type, Namiono::Network::Server* server, int iface,
			Namiono::Network::Client* client, Namiono::Network::Packet* packet)
		{
			client->response = new Packet(type, *packet, packet->get_Length());
			
			switch (client->dhcp->Get_Vendor())
			{
			case PXEClient:
				client->dhcp->SetIsWDSRequest(packet->Has_DHCPOption(static_cast<_BYTE>(250)));
				if (SETTINGS.PXEBOOTMENUE == 1)
				{
					DHCP_Functions::Create_BootServerList(Network::Network::Get_BootServers(), client);
					DHCP_Functions::Generate_Bootmenu_From_ServerList(Network::Network::Get_BootServers(), client);

					// client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_DISCOVERY_CONTROL), static_cast<_BYTE>(SETTINGS.DISCOVERY_MODE));

					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_DISCOVERY_MCADDR),
						server->Get_Interface(iface)->Get_MulticastIP());

					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_MTFTP_IP_ADDR),
						server->Get_Interface(iface)->Get_MulticastIP());

					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_MTFTP_SERVER_PORT), SETTINGS.MTFTP_SPORT);
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_MTFTP_CLIENT_PORT), SETTINGS.MTFTP_CPORT);

					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_DOMAIN), SETTINGS.NBDOMAIN);
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_SERVER), server->Get_Interface(iface)->Get_ServerName());
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_DISCOVERY), static_cast<_BYTE>(1));
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_CONFIGURED), static_cast<_BYTE>(1));
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_VERSION), BS32(static_cast<_UINT>(1)));
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_SERIALNO), std::string("Namiono - Server 0.5"));

					if (client->dhcp->vendorOpts->size() != 0)
						client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(43),
							*client->dhcp->vendorOpts));
				}

				client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(252), client->dhcp->wds->GetBCDfile()));
				break;
			default:
				break;
			}


			DHCP_Functions::Relay_Response_Packet(type, server, iface, client);

			delete client->response;
			client->response = nullptr;
		}

		DHCP_Service::~DHCP_Service()
		{
			upstreamServers.clear();
		}
	}
}