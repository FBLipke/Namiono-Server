#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Services
	{
		void ProxyDHCP_Service::Handle_Service_Request(const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
			Namiono::Network::Client* client, Namiono::Network::Packet* packet)
		{
			if (type != client->Get_ServiceType())
				server->Remove_Client(client->Get_ID());

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

			switch (static_cast<DHCP_MSGTYPE>(packet->Get_DHCPOption(53).Get_Value_As_Byte()))
			{
			case DHCP_MSGTYPE::DISCOVER:
				printf("[I] ProxyDHCP : Request on %s (DISCOVER from %s)...\n", Functions::AddressStr(
					server->Get_Interface(type, iface)->Get_IPAddress()).c_str(),
					packet->get_hwaddress().c_str());
				break;
			case DHCP_MSGTYPE::OFFER:
				break;
			case DHCP_MSGTYPE::REQUEST:
				printf("[I] ProxyDHCP : Request on %s (REQUEST from %s)...\n", Functions::AddressStr(
					server->Get_Interface(type, iface)->Get_IPAddress()).c_str(),
					packet->get_hwaddress().c_str());
				break;
			case DHCP_MSGTYPE::DECLINE:
				printf("[I] ProxyDHCP : Request on %s (DECLINE from %s)...\n", Functions::AddressStr(
					server->Get_Interface(type, iface)->Get_IPAddress()).c_str(),
					packet->get_hwaddress().c_str());
				break;

			case DHCP_MSGTYPE::ACK:
				printf("[I] ProxyDHCP : Request on %s (ACK from %s)...\n", Functions::AddressStr(
					server->Get_Interface(type, iface)->Get_IPAddress()).c_str(),
					packet->get_hwaddress().c_str());
				break;
			case DHCP_MSGTYPE::NAK:
				printf("[I] ProxyDHCP : Request on %s (NAK from %s)...\n", Functions::AddressStr(
					server->Get_Interface(type, iface)->Get_IPAddress()).c_str(),
					packet->get_hwaddress().c_str());
				break;
			case DHCP_MSGTYPE::INFORM:
				printf("[I] ProxyDHCP : Request on %s (INFORM from %s)...\n", Functions::AddressStr(
					server->Get_Interface(type, iface)->Get_IPAddress()).c_str(),
					packet->get_hwaddress().c_str());
				break;
			default:
				break;
			}

			switch (packet->get_opcode())
			{
			case BOOTREQUEST:
				if (!packet->Has_DHCPOption(53))
					return;

				client->Get_DHCP_Client()->SetIsWDSRequest(packet->Has_DHCPOption(250));

				switch (static_cast<DHCP_MSGTYPE>(packet->Get_DHCPOption(53).Get_Value_As_Byte()))
				{
				case INFORM:
				case REQUEST:



					this->Handle_Request_Request(settings, type, server, iface, client, packet);
					break;
				default:
					server->Remove_Client(client->Get_ID());
					return;
				}

				if (client->Get_DHCP_Client()->Get_State() == CLIENTSTATE::DHCP_DONE ||
					client->Get_DHCP_Client()->Get_State() == CLIENTSTATE::DHCP_SERVERRESPONSE ||
					client->Get_DHCP_Client()->Get_State() == CLIENTSTATE::DHCP_ABORT)
					server->Remove_Client(client->Get_ID());
				break;
			}
		}

		void ProxyDHCP_Service::Handle_Request_Request(const SETTINGS* settings, const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
			Namiono::Network::Client* client, Namiono::Network::Packet* packet)
		{
			client->response = new Packet(type, *packet, 1024, DHCP_MSGTYPE::ACK);
			client->Get_DHCP_Client()->SetIsWDSRequest(packet->Has_DHCPOption(250));
			std::vector<DHCP_Option> options;

			_USHORT _type = 0;
			_USHORT _layer = 0;

			std::string _serverName = server->Get_Interface(type, iface)->Get_ServerName();
			std::string _bootfile = "";
			_IPADDR _bootServer = server->Get_Interface(type, iface)->Get_IPAddress();

			switch (client->Get_DHCP_Client()->Get_Vendor())
			{
			case PXEClient:
				DHCP_Functions::Create_BootServerList(Network::Network::Get_BootServers(), client);

				if (packet->Has_DHCPOption(static_cast<_BYTE>(77)) && packet->Has_DHCPOption(static_cast<_BYTE>(175)))
				{
					if (Functions::Compare(packet->Get_DHCPOption(static_cast<_BYTE>(77))
						.Get_Value_As_String().c_str(), "iPXE", 4) ||
						Functions::Compare(packet->Get_DHCPOption(static_cast<_BYTE>(77))
							.Get_Value_As_String().c_str(), "gPXE", 4))
					{
						client->Get_DHCP_Client()->SetIsIPXERequest(true);
						DHCP_Functions::Handle_IPXE_Options(server, iface, client, packet);
					}
				}

				if (client->Get_DHCP_Client()->GetIsWDSRequest())
				{
					options.clear();
					packet->Get_DHCPOption(250).Get_SubOptions(options);

					for (_SIZET i = 0; i < options.size(); i++)
					{
						// WDS Architecture...
						
						if (options.at(i).Option == static_cast<_BYTE>(WDSBP_OPT_ARCHITECTURE))
						{
							DHCP_ARCH arch = DHCP_ARCH::INTEL_X86;
							memcpy(&arch, &options.at(i).Value[0], sizeof(_USHORT));
							_USHORT __arch = LE16(arch);

							client->Get_DHCP_Client()->SetArchitecture(static_cast<DHCP_ARCH>(htons(__arch)));
						}

						// WDS Next Action...

						if (options.at(i).Option == static_cast<_BYTE>(WDSBP_OPT_NEXT_ACTION))
						{
							WDSNBP_OPTION_NEXTACTION value = WDSNBP_OPTION_NEXTACTION::APPROVAL;
							memcpy(&value, &options.at(i).Value[0], sizeof(_USHORT));
							client->Get_DHCP_Client()->Get_WDSClient()->SetNextAction(value);
						}
					}

					if (client->Get_DHCP_Client()->Get_WDSClient()->GetBCDfile().size() != 0)
						client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(252),
							client->Get_DHCP_Client()->Get_WDSClient()->GetBCDfile()));

					client->Get_DHCP_Client()->Get_WDSClient()->SetRequestID(+1);
					client->Get_DHCP_Client()->Set_State(static_cast<CLIENTSTATE>(client->Get_DHCP_Client()->Get_WDSClient()->GetActionDone()) == 0
						? CLIENTSTATE::DHCP_WAITING : CLIENTSTATE::DHCP_DONE);
				}

				if (packet->Has_DHCPOption(43))
				{
					options.clear();
					packet->Get_DHCPOption(43).Get_SubOptions(options);

					// Get the selected Server...

					for (_SIZET i = 0; i < options.size(); i++)
					{
						if (options.at(i).Option == static_cast<_BYTE>(PXE_BOOT_ITEM))
						{
							memcpy(&_type, &options.at(i).Value[0], sizeof(_USHORT));
							if (_type != 0)
								client->Get_DHCP_Client()->Get_RBCPClient()->Set_Item(LE16(_type));

							memcpy(&_layer, &options.at(i).Value[2], sizeof(_USHORT));
							if (_layer != 0)
								client->Get_DHCP_Client()->Get_RBCPClient()->Set_Layer(static_cast<RBCP_LAYER>(_layer));
						}
					}

					_type = client->Get_DHCP_Client()->Get_RBCPClient()->Get_Item();
					_layer = client->Get_DHCP_Client()->Get_RBCPClient()->Get_Layer();

					if (_type != 0)
					{
						if (DHCP_Functions::Has_BootServer(Network::Network::Get_BootServers(), _type))
						{
							std::vector<_IPADDR> addrs = Network::Network::Get_BootServers()->at(_type - 1).Addresses;

							if (addrs.size() > 1)
							{
								for (_SIZET Addridx = 0; Addridx < addrs.size(); Addridx++)
								{
									if (Functions::CompareIPAddress(addrs.at(Addridx), client->Get_Client_Hint().sin_addr.s_addr, 3))
									{
										_bootServer = addrs.at(Addridx);
										break;
									}
								}

								_serverName = Network::Network::Get_BootServers()->at(_type - 1).Description;
								_bootfile = Network::Network::Get_BootServers()->at(_type - 1).Bootfile;

								printf("[I] ProxyDHCP : Redirecting client %s to Server %s (%s)...\n",
									client->Get_ID().c_str(), Functions::AddressStr(_bootServer).c_str(),
									_serverName.c_str());

								client->Get_DHCP_Client()->Get_WDSClient()->SetReferralServer(_bootServer);
							}
							else
							{
								_bootServer = addrs.at(0);
								_bootfile = Network::Network::Get_BootServers()->at(_type - 1).Bootfile;
								_serverName = Network::Network::Get_BootServers()->at(_type - 1).Description;
							}

							char item[4];
							ClearBuffer(item, sizeof item);

							_USHORT __LE_TYPE__ = LE16(_type);
							memcpy(&item[0], &__LE_TYPE__, sizeof  __LE_TYPE__);
							memcpy(&item[2], &_layer, sizeof _layer);
							client->Get_DHCP_Client()->Get_VendorOpts()->clear();

							client->Get_DHCP_Client()->Get_VendorOpts()->emplace_back(
								static_cast<_BYTE>(PXE_BOOT_ITEM), static_cast<_BYTE>(4), LE16(item));
						}
					}
					printf("%d\n", _type);
				}

				client->Get_DHCP_Client()->SetNextServer(_bootServer);
				client->response->set_nextIP(client->Get_DHCP_Client()->GetNextServer());

				if (_serverName.size() == 0)
					_serverName = Functions::AddressStr(client->Get_DHCP_Client()->GetNextServer());
				_serverName = Functions::Replace(_serverName, "(*) ", "");

				client->response->Add_DHCPOption(DHCP_Option(66, _serverName));

				client->response->set_servername(_serverName);

				// client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(1),	static_cast<_ULONG>(server->Get_Interface(type, iface)->Get_Netmask())));

				client->response->Add_DHCPOption(DHCP_Option(54, client->Get_DHCP_Client()->GetNextServer()));
				client->response->Add_DHCPOption(DHCP_Option(150, client->Get_DHCP_Client()->GetNextServer()));

				if (client->Get_DHCP_Client()->Get_VendorOpts()->size() != 0 && packet->Has_DHCPOption(43))
					client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(43),
						*client->Get_DHCP_Client()->Get_VendorOpts()));

				client->response->Add_DHCPOption(DHCP_Option(53, static_cast<_BYTE>(ACK)));
				client->response->Add_DHCPOption(DHCP_Option(60, client->Get_DHCP_Client()->Get_VendorString()));

				if (_type == 1 || !client->response->Has_DHCPOption(43))
				{
					_bootfile = client->Get_DHCP_Client()->GetBootfile();
					DHCP_Functions::Handle_WDS_Options(settings, type, server, iface, client);

					if (client->Get_DHCP_Client()->Get_WDSClient()->GetBCDfile().size() != 0
						&& !client->Get_DHCP_Client()->GetIsWDSResponse())
							client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(252),
								client->Get_DHCP_Client()->Get_WDSClient()->GetBCDfile()));
				}
				else
				{
					if (_type > 1 && _bootfile.size() == 0)
						_bootfile = client->Get_DHCP_Client()->GetBootfile();
				}

				client->response->set_filename(_bootfile);
				break;
			default:
				break;
			}

			client->response->Commit();
			server->Send(type, iface, client);
			client->Get_DHCP_Client()->Set_State(DHCP_DONE);
		}

		ProxyDHCP_Service::ProxyDHCP_Service(SETTINGS* settings)
		{
			this->settings = settings;
		}

		ProxyDHCP_Service::~ProxyDHCP_Service()
		{
		}

		void ProxyDHCP_Service::Start()
		{
		}

		void ProxyDHCP_Service::Close()
		{
		}

		void ProxyDHCP_Service::Heartbeart()
		{
		}

		void ProxyDHCP_Service::Init()
		{
		}
	}
}
