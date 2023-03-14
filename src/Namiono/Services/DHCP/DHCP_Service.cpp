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
		DHCP_Service::DHCP_Service(SETTINGS* settings, const std::vector<DHCP_UPSTREAMSERVER>& upstreamServers)
		{
			this->upstreamServers = upstreamServers;
			this->settings = settings;
		}

		void DHCP_Service::Handle_Service_Request(const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
			Namiono::Network::Client* client, Namiono::Network::Packet* packet)
		{
			if (type != client->Get_ServiceType())
			{
				server->Remove_Client(client->Get_ID());
				return;
			}

			Network::Network::Get_BootServers()->clear();
			std::vector<_IPADDR> addresses;

			if (packet->Has_DHCPOption(60))
			{
				if (Functions::Compare(packet->Get_DHCPOption(static_cast<_BYTE>(60)).Value, "PXEClient", 9))
					client->Get_DHCP_Client()->Set_Vendor(PXEClient);

				if (Functions::Compare(packet->Get_DHCPOption(static_cast<_BYTE>(60)).Value, "PXEServer", 9))
					client->Get_DHCP_Client()->Set_Vendor(PXEServer);

				if (Functions::Compare(packet->Get_DHCPOption(static_cast<_BYTE>(60)).Value, "AAPLBSDPC", 9))
				{
					client->Get_DHCP_Client()->Set_Vendor(AAPLBSDPC);
					client->Get_DHCP_Client()->SetIsBSDPRequest(true);
				}
			}

			switch (packet->get_opcode())
			{
			case BOOTREQUEST:
				if (!packet->Has_DHCPOption(53))
					return;

				switch (static_cast<DHCP_MSGTYPE>(packet->Get_DHCPOption(53).Get_Value_As_Byte()))
				{
				case DHCP_MSGTYPE::DISCOVER:
					printf("[I] DHCP : Request on %s (DISCOVER from %s)...\n", Functions::AddressStr(
						server->Get_Interface(type, iface)->Get_IPAddress()).c_str(),
						packet->get_hwaddress().c_str());
					break;
				case DHCP_MSGTYPE::REQUEST:
					printf("[I] DHCP : Request on %s (REQUEST from %s)...\n", Functions::AddressStr(
						server->Get_Interface(type, iface)->Get_IPAddress()).c_str(),
						packet->get_hwaddress().c_str());
					break;
				case DHCP_MSGTYPE::INFORM:
					printf("[I] DHCP : Request on %s (INFORM from %s)...\n", Functions::AddressStr(
						server->Get_Interface(type, iface)->Get_IPAddress()).c_str(),
						packet->get_hwaddress().c_str());
					break;
				default:
					break;
				}

				switch (static_cast<DHCP_MSGTYPE>(packet->Get_DHCPOption(53).Get_Value_As_Byte()))
				{
				case DISCOVER:
				case INFORM:
				case REQUEST:
					this->Handle_DHCP_Request(type, server, iface, client, packet);
					break;
				default:
					break;
				}
				break;
			case BOOTREPLY:
				if (!packet->Has_DHCPOption(53))
					return;

				client->Get_DHCP_Client()->SetIsWDSResponse(packet->Has_DHCPOption(250));

				switch (static_cast<DHCP_MSGTYPE>(packet->Get_DHCPOption(53).Get_Value_As_Byte()))
				{
				case DHCP_MSGTYPE::OFFER:
					if (client->Get_Server_Hint().sin_addr.s_addr != 0)
					{
						printf("[I] DHCP : Response on %s (OFFER from %s) for %s...\n", Functions::AddressStr(
							server->Get_Interface(type, iface)->Get_IPAddress()).c_str(),
							Functions::AddressStr(client->Get_Server_Hint().sin_addr.s_addr).c_str(),
							packet->get_hwaddress().c_str());
					}
					break;
				case DHCP_MSGTYPE::ACK:
					if (client->Get_Server_Hint().sin_addr.s_addr != 0)
					{
						printf("[I] DHCP : Response on %s (ACK from %s) for %s...\n", Functions::AddressStr(
							server->Get_Interface(type, iface)->Get_IPAddress()).c_str(),
							Functions::AddressStr(client->Get_Server_Hint().sin_addr.s_addr).c_str(),
							packet->get_hwaddress().c_str());
					}
					break;
				case DHCP_MSGTYPE::NAK:
					if (client->Get_Server_Hint().sin_addr.s_addr != 0)
					{
						printf("[I] DHCP : Response on %s (NAK from %s) for %s...\n", Functions::AddressStr(
							server->Get_Interface(type, iface)->Get_IPAddress()).c_str(),
							Functions::AddressStr(client->Get_Server_Hint().sin_addr.s_addr).c_str(),
							packet->get_hwaddress().c_str());
					}
					break;
				case DHCP_MSGTYPE::INFORM:
					if (client->Get_Server_Hint().sin_addr.s_addr != 0)
					{
						printf("[I] DHCP : Response on %s (INFORM from %s) for %s...\n", Functions::AddressStr(
							server->Get_Interface(type, iface)->Get_IPAddress()).c_str(),
							Functions::AddressStr(client->Get_Server_Hint().sin_addr.s_addr).c_str(),
							packet->get_hwaddress().c_str());
					}
					break;
				default:
					break;
				}

				for (Iface& iface : server->Get_Interfaces())
					if (iface.Get_ServiceType() == type)
						addresses.emplace_back(iface.Get_IPAddress());

				BootServerType bsType = BootServerType::PXEBootstrapServer;

				if (client->Get_DHCP_Client()->GetIsWDSRequest() || client->Get_DHCP_Client()->GetIsWDSResponse())
					bsType = BootServerType::WindowsNTBootServer;

				DHCP_Functions::Add_BootServer(Network::Network::Get_BootServers(), bsType, Functions::Get_Hostname(), addresses, "");
				switch (static_cast<DHCP_MSGTYPE>(packet->Get_DHCPOption(53).Get_Value_As_Byte()))
				{
				case ACK:
				case OFFER:
				case INFORM:
				case NAK:
					switch (client->Get_DHCP_Client()->Get_Vendor())
					{
					case PXEClient:
					case PXEServer:
						DHCP_Functions::Add_BootServer_To_ServerList(Network::Network::Get_BootServers(), server, client, packet->get_servername(),
							packet->get_filename());
						break;
					default:
						break;
					}

					this->Handle_DHCP_Response(type, server, iface, client, packet);
					break;
				default:
					break;
				}
				break;
			}

			if (client->Get_DHCP_Client()->Get_State() == CLIENTSTATE::DHCP_DONE ||
				client->Get_DHCP_Client()->Get_State() == CLIENTSTATE::DHCP_SERVERRESPONSE ||
				client->Get_DHCP_Client()->Get_State() == CLIENTSTATE::DHCP_ABORT)
				server->Remove_Client(client->Get_ID());
		}

		void DHCP_Service::Handle_DHCP_Request(const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
			Namiono::Network::Client* client, Namiono::Network::Packet* packet)
		{
			/*
				In Situations where we get relayed packets we should store the other Relay Agent Information,
				so that we can correctly relay the packet to the other Relay Agent. This is needed when we get
				the response from the upstream server. In the Packet all Relay Agent has added RID and CID (82)

				We should store these too so that the downstream relay agent doesnt gets confused
				when we relay back the packet. and put ours in the packet.
			*/

			client->Get_DHCP_Client()->SetIsRelayedPacket(packet->get_relayIP() != 0);

			if (client->Get_DHCP_Client()->GetIsRelayedPacket())
			{
				std::vector<Iface> _interfaces;
				server->Get_Interfaces(type, &_interfaces);

				for (Iface& iF : _interfaces)
				{
					if (iF.Get_IPAddress() == packet->get_relayIP())
						continue;
				
					if (client->Get_DHCP_Client()->GetIsRelayedPacket() && this->relaySessions.find(client->Get_ID()) == this->relaySessions.end())
						this->relaySessions.emplace(client->Get_ID(), DHCP_RELAYSESSION(client->Get_Client_Hint().sin_addr.s_addr, packet->get_relayIP(), iface));
				}
			}

			if (this->upstreamServers.size() != 0)
			{
				client->Get_DHCP_Client()->Set_State(CLIENTSTATE::DHCP_RELAY);
				client->response = new Packet(type, *packet, packet->get_Length());

				for (DHCP_UPSTREAMSERVER& _upstreamserver : this->upstreamServers)
				{
					switch (type)
					{
					case DHCP_SERVER:
					case BINL_SERVER:
						if (client->Get_ServiceType() == server->Get_Interface(type, iface)->Get_ServiceType())
							DHCP_Functions::Relay_Request_Packet(_upstreamserver.Get_IPAddress(), _upstreamserver.Get_Port(), type, server, iface, client);
					}
				}

				delete client->response;
				client->response = nullptr;
			}
			else
			{
				Handle_DHCP_Response(type, server, iface, client, packet);
			}
		}

		void DHCP_Service::Handle_DHCP_Response(const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
			Namiono::Network::Client* client, Namiono::Network::Packet* packet)
		{
			client->response = new Packet(type, *packet, packet->get_Length());

			if (client->Get_DHCP_Client()->GetIsRelayedPacket())
			{
				printf("[I] DHCP : Relayed Response on %s (from %s) for %s...\n", Functions::AddressStr(server->Get_Interface(type, iface)->Get_IPAddress()).c_str(),
					Functions::AddressStr(client->Get_Server_Hint().sin_addr.s_addr).c_str(), packet->get_hwaddress().c_str());
			}

			switch (client->Get_DHCP_Client()->Get_Vendor())
			{
			case PXEServer:
			case PXEClient:
				if (settings->PXEBOOTMENUE == 1)
				{
					if (Network::Network::Get_BootServers()->size() == 0)
					{
						std::vector<_IPADDR> addresses;
						addresses.emplace_back(server->Get_Interface(type, iface)->Get_IPAddress());

						BootServerType bsType = BootServerType::PXEBootstrapServer;

						if (client->Get_DHCP_Client() ->GetIsWDSRequest() || client->Get_DHCP_Client()->GetIsWDSResponse())
							bsType = BootServerType::WindowsNTBootServer;

						DHCP_Functions::Add_BootServer(Network::Network::Get_BootServers(), bsType,
							server->Get_Interface(type, iface)->Get_ServerName(), addresses, client->Get_DHCP_Client()->GetBootfile());
					}

					DHCP_Functions::Create_BootServerList(Network::Network::Get_BootServers(), client);
					DHCP_Functions::Generate_Bootmenu_From_ServerList(settings, Network::Network::Get_BootServers(), client);

					
					RBCP_DISCOVERYCONTROL _control = static_cast<RBCP_DISCOVERYCONTROL>
						(client->Get_DHCP_Client()->Get_RBCPClient()->Get_Control());

					if ((_control != DISABLE_MCAST || _control != UNICAST_ONLY) && settings->MULTICAST_SUPPORT)
					{
						client->Get_DHCP_Client()->Get_VendorOpts()->emplace_back(static_cast<_BYTE>(PXE_DISCOVERY_MCADDR),
							server->Get_Interface(type, iface)->Get_MulticastIP());

						client->Get_DHCP_Client()->Get_VendorOpts()->emplace_back(static_cast<_BYTE>(PXE_MTFTP_IP_ADDR),
							server->Get_Interface(type, iface)->Get_MulticastIP());
					
						client->Get_DHCP_Client()->Get_VendorOpts()->emplace_back(static_cast<_BYTE>(PXE_MTFTP_SERVER_PORT), settings->MTFTP_SPORT);
						client->Get_DHCP_Client()->Get_VendorOpts()->emplace_back(static_cast<_BYTE>(PXE_MTFTP_CLIENT_PORT), settings->MTFTP_CPORT);
					}

					
					if (settings->NBDOMAIN.size() != 0)
						client->Get_DHCP_Client()->Get_VendorOpts()->emplace_back(static_cast<_BYTE>(PXE_LCM_DOMAIN), settings->NBDOMAIN);
					
					client->Get_DHCP_Client()->Get_VendorOpts()->emplace_back(static_cast<_BYTE>(PXE_LCM_SERVER), server->Get_Interface(type, iface)->Get_ServerName());
					client->Get_DHCP_Client()->Get_VendorOpts()->emplace_back(static_cast<_BYTE>(PXE_LCM_DISCOVERY), static_cast<_BYTE>(1));
					client->Get_DHCP_Client()->Get_VendorOpts()->emplace_back(static_cast<_BYTE>(PXE_LCM_CONFIGURED), static_cast<_BYTE>(1));
					client->Get_DHCP_Client()->Get_VendorOpts()->emplace_back(static_cast<_BYTE>(PXE_LCM_VERSION), LE32(static_cast<_UINT>(1)));
					client->Get_DHCP_Client()->Get_VendorOpts()->emplace_back(static_cast<_BYTE>(PXE_LCM_SERIALNO), std::string("Namiono - Server 0.5"));
				/*
					if (client->Get_DHCP_Client()->Get_VendorOpts()->size() != 0)
						client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(43),
							*client->Get_DHCP_Client()->Get_VendorOpts()));
				*/
				}

				if (client->Get_DHCP_Client()->Get_WDSClient()->GetBCDfile().size() != 0 && !client->Get_DHCP_Client()->GetIsWDSResponse())
					client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(252), client->Get_DHCP_Client()->Get_WDSClient()->GetBCDfile()));
				break;
			case AAPLBSDPC:
				if (client->Get_DHCP_Client()->GetIsBSDPRequest())
					return;

				// Apple Clients requests sometimes a specific Reply Port...
				client->Set_Port(client->Get_DHCP_Client()->Get_BSDPClient()->Get_ReplyPort());
			default:
				break;
			}

			client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(255)));
			client->Get_DHCP_Client()->Set_State(CLIENTSTATE::DHCP_CLIENTRESPONSE);

			if (client->Get_DHCP_Client()->GetIsRelayedPacket())
			{
				DHCP_Functions::Relay_Response_Packet(&this->relaySessions, type, server, iface, client, packet);
				client->SetIncomingInterface(DHCP_Functions::Handle_Relayed_Packet(type, server, iface, client->response));
			}
			else
			{
				client->Set_Client_Hint(packet->get_clientIP() == 0 ? INADDR_BROADCAST : packet->get_clientIP(), client->Get_Port());
				client->response->set_opcode(DHCP_OPCODE::BOOTREPLY);
				client->response->set_flags(packet->get_flags());
				client->response->set_nextIP(server->Get_Interface(type, client->GetIncomingInterface())->Get_IPAddress());
				client->response->set_servername(server->Get_Interface(type, client->GetIncomingInterface())->Get_ServerName());
#ifndef _WIN32
				client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(54),
					static_cast<_IPADDR>(server->Get_Interface(type, client->GetIncomingInterface())->Get_IPAddress())));
#else
				client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(54), 
					static_cast<_IPADDR>(server->Get_Interface(type, client->GetIncomingInterface())->Get_IPAddress())));
#endif
				// Remove the Request List!
				client->response->Remove_DHCPOption(55);
				client->response->set_filename(client->Get_DHCP_Client()->GetBootfile());
				client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(53), static_cast<_BYTE>(DHCP_MSGTYPE::OFFER)));
				client->response->Commit();

				if (server->Get_Interface(type, client->GetIncomingInterface())->Get_ServiceType() == type)
					server->Send(type, client->GetIncomingInterface(), client);
			}

			client->Get_DHCP_Client()->Set_State(DHCP_DONE);
			delete client->response;
			client->response = nullptr;
		}

		DHCP_Service::~DHCP_Service()
		{
			upstreamServers.clear();
		}
		
		void DHCP_Service::Start()
		{
		}
		
		void DHCP_Service::Close()
		{
		}
		
		void DHCP_Service::Heartbeart()
		{
		}

		void DHCP_Service::Init()
		{
		}
	}
}