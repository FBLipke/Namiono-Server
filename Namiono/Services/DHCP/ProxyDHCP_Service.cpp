#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Services
	{
		void ProxyDHCP_Service::Handle_Service_Request(const ServiceType & type, Namiono::Network::Server * server, int iface,
			Namiono::Network::Client * client, Namiono::Network::Packet * packet)
		{
			printf("[I] ProxyDHCP : Request on %s...\n",
				Functions::AddressStr(server->Get_Interface(iface)->Get_IPAddress()).c_str());

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

				client->dhcp->SetIsWDSRequest(packet->Has_DHCPOption(250));

				switch (static_cast<DHCP_MSGTYPE>(packet->Get_DHCPOption(53).Get_Value_As_Byte()))
				{
				case INFORM:
				case REQUEST:
					this->Handle_Request_Request(type, server, iface, client, packet);
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

		void ProxyDHCP_Service::Handle_Request_Request(const ServiceType & type, Namiono::Network::Server * server, int iface,
			Namiono::Network::Client * client, Namiono::Network::Packet * packet)
		{
			client->response = new Packet(type, *packet, 1024, DHCP_MSGTYPE::ACK);
			client->dhcp->SetIsWDSRequest(packet->Has_DHCPOption(250));
			std::vector<DHCP_Option> options;
			switch (client->dhcp->Get_Vendor())
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
						client->dhcp->SetIsIPXERequest(true);
						DHCP_Functions::Handle_IPXE_Options(server, iface, client, packet);
					}
				}

				if (client->dhcp->GetIsWDSRequest())
				{
					packet->Get_DHCPOption(250).Get_SubOptions(options);

					for (_SIZET i = 0; i < options.size(); i++)
						if (options.at(i).Option == static_cast<_BYTE>(WDSBP_OPT_ARCHITECTURE))
						{
							DHCP_ARCH arch = INTEL_X86;

							memcpy(&arch, &options.at(i).Value[0], sizeof(_USHORT));
							client->dhcp->SetArchitecture(arch);
						}

					client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(252), client->dhcp->wds->GetBCDfile()));

					client->dhcp->wds->SetRequestID(+1);
					client->dhcp->Set_State(client->dhcp->wds->GetActionDone() == 0 ? DHCP_WAITING : DHCP_DONE);
				}

				DHCP_Functions::Handle_WDS_Options(server, iface, client);

				if (packet->Has_DHCPOption(43))
				{
					packet->Get_DHCPOption(43).Get_SubOptions(options);

					// Get the selected Server...

					for (_SIZET i = 0; i < options.size(); i++)
					{
						if (options.at(i).Option == static_cast<_BYTE>(PXE_BOOT_ITEM))
						{
							RBCP_LAYER layer = RBCP_LAYER::Bootfile;
							_USHORT _type = 0;

							memcpy(&_type, &options.at(i).Value[0], sizeof(_USHORT));
							if (_type != 0)
								client->dhcp->rbcp->Set_Item(_type);

							memcpy(&layer, &options.at(i).Value[2], sizeof(_USHORT));
							if (layer != 0)
								client->dhcp->rbcp->Set_Layer(layer);
						}
					}
				}

				_serverName = server->Get_Interface(iface)->Get_ServerName();
				_bootServer = server->Get_Interface(iface)->Get_IPAddress();

				if (client->dhcp->rbcp->Get_Item() != 0)
				{
					char item[4];
					ClearBuffer(item, sizeof item);
					_USHORT _type = client->dhcp->rbcp->Get_Item();
					RBCP_LAYER layer = client->dhcp->rbcp->Get_Layer();

					memcpy(&item[0], &_type, sizeof(_USHORT));
					memcpy(&item[2], &layer, sizeof(_USHORT));

					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_BOOT_ITEM), static_cast<_BYTE>(4), item);


					if (DHCP_Functions::Has_BootServer(Network::Network::Get_BootServers(), _type))
					{
						std::vector<_IPADDR> addrs = Network::Network::Get_BootServers()->at(_type - 1).Addresses;

						if (addrs.size() > 1)
						{
							for (_SIZET Addridx = 0; Addridx < addrs.size(); Addridx++)
							{
								if (Functions::CompareIPAddress(addrs.at(Addridx),
									client->Get_Client_Hint().sin_addr.s_addr, 3))
								{
									_bootServer = addrs.at(Addridx);
								}
							}
						}
						else
						{
							_bootServer = addrs.at(0);
						}

						if (_type > 0)
						{
							_serverName = Network::Network::Get_BootServers()->at(_type - 1).Description;

							printf("[I] ProxyDHCP : Redirecting client %s to Server %s (%s:4011)...\n",
								client->Get_ID().c_str(), Functions::AddressStr(_bootServer).c_str(),
								_serverName.c_str(), server->Get_Interface(iface)->Get_Port());

							if (server->Get_Interface(iface)->Get_Port() == 4011)
							{
								client->response->set_yourIP(client->response->get_clientIP());
								client->response->set_clientIP(0);
							}
						}
					}
					else
					{
						printf("[W] ProxyDHCP : The requested Bootserver (%d) from %s does not exist!\n", _type, client->Get_ID().c_str());
					}
				}

				client->dhcp->SetNextServer(_bootServer);
				client->response->set_nextIP(_bootServer);

				client->response->Add_DHCPOption(DHCP_Option(66, _serverName));
				client->response->set_servername(_serverName);

				client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(1),
					static_cast<_ULONG>(server->Get_Interface(iface)->Get_Netmask())));

				client->response->Add_DHCPOption(DHCP_Option(54, client->dhcp->GetNextServer()));
				client->response->Add_DHCPOption(DHCP_Option(150, client->dhcp->GetNextServer()));

				if (client->dhcp->vendorOpts->size() != 0)
					client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(43),
						*client->dhcp->vendorOpts));

				client->response->Add_DHCPOption(DHCP_Option(53, static_cast<_BYTE>(ACK)));
				client->response->Add_DHCPOption(DHCP_Option(60, client->dhcp->Get_VendorString()));

				if (client->dhcp->rbcp->Get_Item() == 1)
					client->response->set_filename(client->dhcp->GetBootfile());
				break;
			default:
				break;
			}

			client->response->Commit();

			// Sometimes we responding too fast, so wait here... x ms;

			server->Send(iface, client);
		}

		ProxyDHCP_Service::ProxyDHCP_Service()
		{
		}

		ProxyDHCP_Service::~ProxyDHCP_Service()
		{
		}
	}
}
