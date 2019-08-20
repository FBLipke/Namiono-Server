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

#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Network
	{
		_ULONG requestId = 0;
		_USHORT id = 1;
		std::vector<std::thread> servers;
		std::vector<_IPADDR> addresses;

#ifdef _WIN32
		bool Init_Winsock(_INT32 major, _INT32 minor)
		{
			WSADATA wsa;
			ClearBuffer(&wsa, sizeof wsa);

			return WSAStartup(MAKEWORD(major, minor), &wsa) == 0;
		}

		bool Close_Winsock()
		{
			return WSACleanup() == 0;
		}
#endif
		std::string Get_Hostname()
		{
			char hname[64];
			ClearBuffer(hname, sizeof hname);
			_INT32 retval = gethostname(hname, sizeof hname);
			if (retval == SOCKET_ERROR)
			{
				printf("[E] gethostname(): %s\n", strerror(errno));
				return "";
			}

			return std::string(hname);
		}

		void add_server(_IPADDR address, _IPADDR gateway, _IPADDR netmask, const std::string& tftp_rootDir)
		{
			servers.emplace_back(Namiono::Network::__serverThread<sockaddr_in>,
				DHCP_SERVER, AF_INET, false, address, gateway, netmask, 67, tftp_rootDir, Handle_Request<sockaddr_in>);

			servers.emplace_back(Namiono::Network::__serverThread<sockaddr_in>,
				BINL_SERVER, AF_INET, SETTINGS.MULTICAST_SUPPORT, address, netmask, gateway, 4011, tftp_rootDir, Handle_Request<sockaddr_in>);

			servers.emplace_back(Namiono::Network::__serverThread<sockaddr_in>,
				TFTP_SERVER, AF_INET, false, address, gateway, netmask, 69, tftp_rootDir, Handle_Request<sockaddr_in>);

			if (SETTINGS.MULTICAST_SUPPORT)
			{
				servers.emplace_back(Namiono::Network::__serverThread<sockaddr_in>,
					TFTP_SERVER, AF_INET, SETTINGS.MULTICAST_SUPPORT, address, gateway, netmask, SETTINGS.MTFTP_SPORT, tftp_rootDir, Handle_Request<sockaddr_in>);
			}
		}

		bool Get_Interface_Addresses(const std::string& rootDir, std::function<void(_IPADDR, _IPADDR, _IPADDR, const std::string& rootDir)> cb)
		{
			_IPADDR address = 0;
			_IPADDR gateway = 0;
			_IPADDR ipMask = 0;
#ifdef _WIN32
			PIP_ADAPTER_INFO pAdapterInfo = nullptr;
			PIP_ADAPTER_INFO pAdapter = nullptr;
			_ULONG ulOutBufLen = 0;

			if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
			{
				free(pAdapterInfo);
				pAdapterInfo = static_cast<IP_ADAPTER_INFO*>(malloc(ulOutBufLen));
				if (pAdapterInfo == nullptr)
					return false;
			}

			if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
			{
				pAdapter = pAdapterInfo;
				while (pAdapter)
				{
					address = inet_addr(pAdapter->IpAddressList.IpAddress.String);
					gateway = inet_addr(pAdapter->GatewayList.IpAddress.String);
					ipMask = inet_addr(pAdapter->IpAddressList.IpMask.String);

					if (address != 0)
						addresses.emplace_back(address);

					cb(address, gateway, ipMask, rootDir);

					pAdapter = pAdapter->Next;
				}

				pAdapter = nullptr;
			}

			if (pAdapterInfo)
				free(pAdapterInfo);

			pAdapterInfo = nullptr;
#else
			ifaddrs* ifap = nullptr, *ifa = nullptr;
			getifaddrs(&ifap);
			for (ifa = ifap; ifa; ifa = ifa->ifa_next)
			{
				if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET)
				{
					sockaddr_in* sa = (struct sockaddr_in*) ifa->ifa_addr;
					address = sa->sin_addr.s_addr;
					addresses.emplace_back(address);
					cb(address, gateway, ipMask, rootDir);				}
			}

			freeifaddrs(ifap);
			ifap = nullptr;
#endif



			bool haveAddresses = addresses.size() != 0;
			return haveAddresses;
		}

		void Bootstrap_Network(const std::string & tftp_rootDir)
		{
			printf("[I] Starting network...\n");

#ifdef _WIN32
			if (!Init_Winsock(2, 2))
			{
				printf("[E] Error: %s\n", strerror(WSAGetLastError()));
				return;
			}
#endif

			if (Get_Interface_Addresses(tftp_rootDir, add_server))
			{
				for (std::thread& serverThread : servers)
					serverThread.join();
			}

#ifdef _WIN32
			if (!Close_Winsock())
			{
				printf("[E] Closing Winsock - Error: %d\n", WSAGetLastError());
				return;
			}
#endif
		}

		template<class S>
		void Handle_Request(ServiceType * type, Server<S> * server,
			Client<S> * client, const std::string & rootDir, Packet * packet)
		{
			switch (*type)
			{
			case BINL_SERVER:
			case DHCP_SERVER:
				switch (packet->Get_Opcode())
				{
				case DHCP_REQ:
				case DHCP_RES:
					if (!packet->Has_DHCPOption(static_cast<_BYTE>(60)))
					{
						packet->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(60), ""));
					}
					
					if (Functions::Compare(packet->Get_DHCPOption(static_cast<_BYTE>(60)).Value, "PXEClient", 9))
					{
						client->dhcp->Set_Vendor(PXEClient);

						if (packet->get_clientIP() == 0 && packet->get_opcode() == BOOTREQUEST)
						{
							Relay_Request_Packet(type, server, client, packet);
						}
					}
					else
					{
						if (Functions::Compare(packet->Get_DHCPOption(static_cast<_BYTE>(60)).Value, "PXEServer", 9))
							client->dhcp->Set_Vendor(PXEServer);
						else
						{
							if (Functions::Compare(packet->Get_DHCPOption(static_cast<_BYTE>(60)).Value, "AAPLBSDPC", 9))
							{
								client->dhcp->Set_Vendor(AAPLBSDPC);
								client->dhcp->SetIsBSDPRequest(true);
							}
							else
							{
								client->dhcp->SetIsRelayedPacket(packet->get_relayIP() != 0);

								if (client->dhcp->GetIsRelayedPacket())
								{
									if (packet->get_opcode() == BOOTREPLY)
									{
										Relay_Response_Packet(type, server, client, packet);
										return;
									}
								}
								else
								{
									if (packet->get_opcode() == BOOTREQUEST)
									{
										Relay_Request_Packet(type, server, client, packet);
										return;
									}
								}
							}
						}
					}

					client->dhcp->SetIsWDSRequest(packet->Has_DHCPOption(static_cast<_BYTE>(250)));

					switch (static_cast<DHCP_MSGTYPE>(packet->Get_DHCPOption(static_cast<_BYTE>(53))
						.Get_Value_As_Byte()))
					{
					case DHCP_MSGTYPE::DISCOVER:
						Handle_DHCP_Discover(type, server, client, packet);
						break;
					case DHCP_MSGTYPE::INFORM:
					case DHCP_MSGTYPE::REQUEST:
						Handle_DHCP_Request(type, server, client, packet);
						break;
					}

					if (client->dhcp->Get_State() == CLIENTSTATE::DHCP_DONE ||
						client->dhcp->Get_State() == CLIENTSTATE::DHCP_ABORT)
						server->Remove_Client(client->Get_ID());
					break;
				case BINL_RQU:
					Handle_RIS_RQU(type, server, client, packet, rootDir);
					break;
				case BINL_NEG:
					Handle_RIS_NEG(type, server, client, packet);
					break;
				default:
					break;
				}

				break;
			case TFTP_SERVER:
				switch (static_cast<TFTP_OPCODE>(packet->Get_Opcode()))
				{
				case Packet_OPCode::TFTP_RRQ:
					Handle_TFTP_RRQ(type, server, client, rootDir, packet);
					break;
				case Packet_OPCode::TFTP_ACK:
					Handle_TFTP_ACK(type, server, client, packet);
					break;
				case Packet_OPCode::TFTP_ERR:
					Handle_TFTP_ERR(type, server, client, packet);
					break;
				}

				if (client->tftp->Get_State() == CLIENTSTATE::TFTP_DONE
					|| client->tftp->Get_State() == CLIENTSTATE::TFTP_ERROR)
					server->Remove_Client(client->Get_ID());
				break;
			}
		}



		template<class S>
		void Relay_Request_Packet(ServiceType * type, Server<S> * server, Client<S> * client, Packet * packet)
		{
			/* Client request */
			client->dhcp->Set_State(CLIENTSTATE::DHCP_RELAY);
			client->Set_Relay_Hint(inet_addr("10.20.0.1"), 67);
			packet->set_relayIP(server->Get_IPAddress());
			packet->set_flags(DHCP_FLAGS::Unicast);
			packet->increase_hops(1);
			client->response = packet;
			server->Send(client);
		}

		template<class S>
		void Relay_Response_Packet(ServiceType * type, Server<S> * server, Client<S> * client, Packet * packet)
		{
			/* Server response */
			client->response = packet;
			client->Set_Client_Hint(INADDR_BROADCAST, 68);

			packet->set_flags(DHCP_FLAGS::Broadcast);

			server->Send(client);
			client->dhcp->Set_State(CLIENTSTATE::DHCP_DONE);
		}

		template<class S>
		void Handle_RIS_RQU(ServiceType * type, Server<S> * server, Client<S> * client, Packet * packet, const std::string& rootDir)
		{
			std::string oscfile = Combine(rootDir, Combine(std::string("OSChooser"), Combine(std::string("English"),
				packet->get_osc_filename(rootDir).c_str())));

			printf("[I] OSC File: %s\n", oscfile.c_str());
			printf("[I] OSC Client: %s\n", client->Get_ID().c_str());


			FILE* fil = fopen(oscfile.c_str(), "rb");
			if (fil != nullptr)
			{
				_SIZET fsize = FileLength(oscfile);
				char _osc_content[65584];
				ClearBuffer(_osc_content, sizeof _osc_content);
				std::string osc_content = "";

				if (FileRead(_osc_content, fsize, fil) != 0)
				{
					osc_content = std::string(_osc_content);
					osc_content = Functions::Replace(osc_content, "%MACHINENAME%", "Client01");

					client->response = new Packet(type, osc_content.size() + 36, BINL_RSU);
					client->response->CopyFrom(*packet, 8, 8, 28);
					client->response->Write(osc_content.c_str(), osc_content.size(), 28);
					client->response->Commit();

					if (server->Send(client) == SOCKET_ERROR)
					{
						print_Error("Failed to send BINL Packet!\n");
					}

					delete client->response;
					client->response = nullptr;


					fclose(fil);
				}
			}
			else
			{
				print_Error("The File was not found!");
			}
		}

		template<class S>
		void Handle_RIS_NEG(ServiceType * type, Server<S>* server, Client<S>* client, Packet * packet)
		{
		}

		template<class S>
		void Handle_RIS_AUT(ServiceType * type, Server<S>* server, Client<S>* client, Packet * packet)
		{
		}

		template<class S>
		void Handle_IPXE_Options(Server<S> * server, Client<S> * client, Packet * response)
		{
			std::vector<DHCP_Option> options;
			response->Get_DHCPOption(static_cast<_BYTE>(175)).Get_SubOptions(options);

			for (_SIZET i = 0; i < options.size(); i++)
			{
				IPXE_Options opt = static_cast<IPXE_Options>(options.at(i).Option);

				switch (opt)
				{
				case IPXE_USERNAME:
					client->dhcp->ipxe->Set_Username(options.at(i).Get_Value_As_String());
					break;
				case IPXE_PASSWORD:
					client->dhcp->ipxe->Set_Password(options.at(i).Get_Value_As_String());
					break;
				case IPXE_BUSID:
					break;
				}
			}
		}

		template<class S>
		void Handle_WDS_Options(Server<S> * server, Client<S> * client)
		{
			// WDS Option -> Used by WDSNBP
			std::vector<DHCP_Option>* wdsOptions = new std::vector<DHCP_Option>();
			wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_NEXT_ACTION), static_cast<_BYTE>(client->dhcp->wds->GetNextAction()));
			wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_REQUEST_ID), static_cast<_ULONG>(BS32(client->dhcp->wds->GetRequestID())));

			wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_POLL_INTERVAL), static_cast<_USHORT>(BS16(client->dhcp->wds->GetPollInterval())));
			wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_POLL_RETRY_COUNT), static_cast<_USHORT>(BS16(client->dhcp->wds->GetRetryCount())));

			if (client->dhcp->wds->GetNextAction() == REFERRAL)
			{
				client->dhcp->wds->SetReferralServer(server->Get_IPAddress());
				wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_REFERRAL_SERVER), client->dhcp->wds->GetReferalServer());
				wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_ALLOW_SERVER_SELECTION), static_cast<_BYTE>(1));
			}

			wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_ACTION_DONE), static_cast<_BYTE>(client->dhcp->wds->GetActionDone()));

			if (client->dhcp->wds->GetNextAction() == APPROVAL && client->dhcp->wds->GetWDSMessage().size() != 0)
			{
				wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_MESSAGE), client->dhcp->wds->GetWDSMessage());
			}

			client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(250), *wdsOptions));
			client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(255)));

			wdsOptions->clear();
			delete wdsOptions;
			wdsOptions = nullptr;

		}

		template<class S>
		void GenerateBootServers(Client<S> * client)
		{
			client->dhcp->Set_State(DHCP_INIT);
			client->dhcp->rbcp->Clear_BootServers();
			client->dhcp->rbcp->Add_BootServer(Get_Hostname(), addresses);

			FILE* fil = fopen("Config/servers.txt", "r");

			if (fil != nullptr)
			{
				char line[1024];
				ClearBuffer(line, sizeof line);
				while (fgets(line, sizeof line, fil) != nullptr)
				{
					char desc[64];
					ClearBuffer(desc, 64);
					char addr[1024];
					ClearBuffer(addr, 1024);

					std::vector<_IPADDR> addrs;

					if (sscanf(line, "%s | %s", &addr, &desc) != 0)
					{
						std::string _addrline = std::string(addr);
						std::vector<std::string> _addrs = Functions::Split(_addrline, std::string(","));

						for (size_t i = 0; i < _addrs.size(); i++)
						{
							addrs.emplace_back(inet_addr(_addrs.at(i).c_str()));
						}

						client->dhcp->rbcp->Add_BootServer(std::string(desc), addrs);
					}
				}

				fclose(fil);
			}
		}

		template<class S>
		void Handle_DHCP_Discover(ServiceType * type, Server<S> * server, Client<S> * client, Packet * packet)
		{
			switch (client->dhcp->Get_Vendor())
			{
			case PXEClient:
				if (SETTINGS.PXEBOOTMENUE == 1)
				{
					GenerateBootServers(client);
					Create_BootServer_List(client);
					GenerateBootMenue(client);

					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>
						(PXE_DISCOVERY_CONTROL), static_cast<_BYTE>(SETTINGS.DISCOVERY_MODE));

					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_DISCOVERY_MCADDR),
						static_cast<_IPADDR>(inet_addr(SETTINGS.DISCOVERY_ADDR.c_str())));

					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_MTFTP_IP_ADDR),
						static_cast<_IPADDR>(inet_addr(SETTINGS.DISCOVERY_ADDR.c_str())));

					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_MTFTP_SERVER_PORT), SETTINGS.MTFTP_SPORT);
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_MTFTP_CLIENT_PORT), SETTINGS.MTFTP_CPORT);

					/* LCM related Options
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_DOMAIN), SETTINGS.NBDOMAIN);
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_SERVER), server->Get_ServerName());
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_DISCOVERY), static_cast<_BYTE>(1));
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_CONFIGURED), static_cast<_BYTE>(1));
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_VERSION), BS32(static_cast<_UINT>(1)));
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_SERIALNO), std::string("Namiono - Server 0.5"));
 */
				}

				client->response = new Packet(type, *packet, 1024, DHCP_MSGTYPE::OFFER);
				if (client->dhcp->vendorOpts->size() != 0)
					client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(43),
						*client->dhcp->vendorOpts));

				client->response->set_servername(server->Get_ServerName());
				client->response->set_nextIP(server->Get_IPAddress());

				client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(1), static_cast<_UINT>(server->Get_NetMask())));
				client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(3), static_cast<_UINT>(server->Get_Gateway())));
				client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(53), static_cast<_BYTE>(OFFER)));
				client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(54), static_cast<_UINT>(client->dhcp->GetNextServer())));
				client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(60), client->dhcp->Get_VendorString()));

				Handle_WDS_Options(server, client);
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

			int retval = server->Send(client);
			if (retval == SOCKET_ERROR)
				printf("[E] Error while sending %s packet! (%s)\n",
					client->Get_TypeString().c_str(), strerror(errno));

			delete client->response;
			client->response = nullptr;

			client->dhcp->Set_State(DHCP_DONE);
		}

		template<class S>
		void Handle_DHCP_Request(ServiceType * type, Server<S> * server, Client<S> * client, Packet * packet)
		{
			std::vector<DHCP_Option> options;
			std::string _serverName = "";
			_IPADDR _bootServer = 0;

			client->response = new Packet(type, *packet, 1024, DHCP_MSGTYPE::ACK);
			client->dhcp->SetIsWDSRequest(packet->Has_DHCPOption(250));

			switch (client->dhcp->Get_Vendor())
			{
			case PXEClient:

				GenerateBootServers(client);
				if (packet->Has_DHCPOption(static_cast<_BYTE>(77)) && packet->Has_DHCPOption(static_cast<_BYTE>(175)))
				{
					if (Functions::Compare(packet->Get_DHCPOption(static_cast<_BYTE>(77))
						.Get_Value_As_String().c_str(), "iPXE", 4) ||
						Functions::Compare(packet->Get_DHCPOption(static_cast<_BYTE>(77))
							.Get_Value_As_String().c_str(), "gPXE", 4))
					{
						client->dhcp->SetIsIPXERequest(true);
						Handle_IPXE_Options(server, client, packet);
					}
				}

				if (client->dhcp->GetIsWDSRequest() && client->dhcp->wds->GetNextAction() == APPROVAL)
				{
					packet->Get_DHCPOption(250).Get_SubOptions(options);

					for (_SIZET i = 0; i < options.size(); i++)
						if (options.at(i).Option == static_cast<_BYTE>(WDSBP_OPT_ARCHITECTURE))
						{
							DHCP_ARCH arch = INTEL_X86;

							memcpy(&arch, &options.at(i).Value[0], sizeof(_USHORT));
							client->dhcp->SetArchitecture(arch);
						}

					requestId++;
					client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(252), client->dhcp->wds->GetBCDfile()));

					client->dhcp->wds->SetRequestID(requestId);
					client->dhcp->Set_State(client->dhcp->wds->GetActionDone() == 0 ? DHCP_WAITING : DHCP_DONE);


				}

				Handle_WDS_Options(server, client);

				if (client->dhcp->Get_State() == DHCP_WAITING)
					return;

				if (packet->Has_DHCPOption(43))
				{

					packet->Get_DHCPOption(43).Get_SubOptions(options);

					// Get the selected Server...

					for (_SIZET i = 0; i < options.size(); i++)
					{
						if (options.at(i).Option == static_cast<_BYTE>(PXE_BOOT_ITEM))
						{
							RBCP_LAYER layer = RBCP_LAYER::Bootfile;
							_USHORT type = 0;

							memcpy(&type, &options.at(i).Value[0], sizeof(_USHORT));
							if (type != 0)
								client->dhcp->rbcp->Set_Item(type);

							memcpy(&layer, &options.at(i).Value[2], sizeof(_USHORT));
							if (layer != 0)
								client->dhcp->rbcp->Set_Layer(layer);
						}
					}
				}

				_serverName = server->Get_ServerName();
				_bootServer = inet_addr(client->_socketID.c_str());

				if (client->dhcp->rbcp->Get_Item() != 0)
				{
					char item[4];
					ClearBuffer(item, sizeof item);
					_USHORT type = client->dhcp->rbcp->Get_Item();
					RBCP_LAYER layer = client->dhcp->rbcp->Get_Layer();


					memcpy(&item[0], &type, sizeof(_USHORT));
					memcpy(&item[2], &layer, sizeof(_USHORT));

					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_BOOT_ITEM), static_cast<_BYTE>(4), item);


					if (client->dhcp->rbcp->Has_BootServer(type))
					{
						std::vector<_IPADDR> addrs = client->dhcp->rbcp->Get_Bootservers()->at(type).Addresses;

						if (addrs.size() > 1)
						{
							for (_SIZET Addridx = 0; Addridx < addrs.size(); Addridx++)
							{
								if (Functions::CompareIPAddress(addrs.at(Addridx), client->Get_Client_Hint().sin_addr.s_addr, 3))
								{
									_bootServer = addrs.at(Addridx);
								}
							}
						}
						else
						{
							_bootServer = addrs.at(0);
						}

						_serverName = client->dhcp->rbcp->Get_Bootservers()->at(type).Description;

						printf("[D] Redirecting client %s to Server %s (%s)...\n", client->Get_ID().c_str(), Functions::AddressStr(_bootServer).c_str(),
							_serverName.c_str());

						if (server->Get_Port() == 4011)
						{
							client->response->set_yourIP(client->response->get_clientIP());
							client->response->set_clientIP(0);
						}
					}
					else
					{
						printf("The requested Bootserver (%d) from %s does not exist!\n", type, client->Get_ID().c_str());
					}
				}

				client->dhcp->SetNextServer(_bootServer);
				client->response->set_nextIP(client->dhcp->GetNextServer());

				client->response->Add_DHCPOption(DHCP_Option(66, _serverName));
				client->response->set_servername(_serverName);

				client->response->Add_DHCPOption(DHCP_Option(1, server->Get_NetMask()));
				client->response->Add_DHCPOption(DHCP_Option(3, server->Get_Gateway()));
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
#ifdef _WIN32
			Sleep(static_cast<_BYTE>((1 + SETTINGS.SUBNETDELAY)));
#else
			sleep(static_cast<_BYTE>((1 + SETTINGS.SUBNETDELAY)));
#endif

			int retval = server->Send(client);
			if (retval == SOCKET_ERROR)
				printf("Error while sending %s packet! (%s)\n",
					client->Get_TypeString().c_str(), strerror(errno));

			delete client->response;
			client->response = nullptr;
		}

		template<class S>
		void GenerateBootMenue(Client<S> * client)
		{
			_SIZET offset = 0;
			char menubuffer[1024];

			ClearBuffer(menubuffer, sizeof menubuffer);

			/* id */
			_USHORT id = 0;
			memcpy(&menubuffer[offset], &id, sizeof id);
			offset += sizeof id;

			/* desc len */
			_BYTE length = static_cast<_BYTE>(SETTINGS.PXEHDDDESC.size());
			memcpy(&menubuffer[offset], &length, sizeof length);
			offset += sizeof length;

			/* desc */
			memcpy(&menubuffer[offset], SETTINGS.PXEHDDDESC.c_str(), length);
			offset += length;

			for (_USHORT i = 0; i < client->dhcp->rbcp->Get_Bootservers()->size(); i++)
			{
				if (client->dhcp->rbcp->Get_Bootservers()->at(i).Description.size() == 0)
					continue;

				/* id */
				id = client->dhcp->rbcp->Get_Bootservers()->at(i).Ident;
				memcpy(&menubuffer[offset], &id, sizeof id);
				offset += sizeof id;

				/* desc len */
				length = static_cast<_BYTE>(strlen(client->dhcp->rbcp->Get_Bootservers()->at(i).Description.c_str()));
				memcpy(&menubuffer[offset], &length, sizeof length);
				offset += sizeof length;

				/* desc */
				memcpy(&menubuffer[offset], client->dhcp->rbcp->Get_Bootservers()->at(i).Description.c_str(), length);
				offset += length;
			}

			client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_BOOT_MENU),
				static_cast<_BYTE>(offset), menubuffer);

			/* Menue prompt */
			_BYTE timeout = SETTINGS.PXEPROMPTTIMEOUT;
			const std::string prompt = timeout != static_cast<_BYTE>(0xff) ?
				SETTINGS.PXEPROMP : std::string("Select Server...");

			char* promptbuffer = new char[prompt.size() + 1];
			ClearBuffer(promptbuffer, prompt.size() + 1);
			offset = 0;

			memcpy(&promptbuffer[offset], &timeout, sizeof timeout);
			offset += sizeof timeout;

			memcpy(&promptbuffer[offset], prompt.c_str(),
				static_cast<_SIZET>(prompt.size()));

			offset += static_cast<_SIZET>(prompt.size());

			client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_MENU_PROMPT),
				static_cast<_BYTE>(offset), promptbuffer);

			delete[] promptbuffer;
			promptbuffer = nullptr;
		}

		template<class S>
		void Create_BootServer_List(Client<S> * client)
		{
			_BYTE offset = 0;
			_BYTE ipcount = 0;
			char* serverbuffer = new char[1024];
			ClearBuffer(serverbuffer, 1024);

			for (_USHORT i = 0; i < client->dhcp->rbcp->Get_Bootservers()->size(); i++)
			{
				if (client->dhcp->rbcp->Get_Bootservers()->at(i).Addresses.size() == 0 ||
					client->dhcp->rbcp->Get_Bootservers()->at(i).Description.size() == 0)
					continue;

				memcpy(&serverbuffer[offset], &client->dhcp->rbcp->Get_Bootservers()->at(i).Ident,
					sizeof client->dhcp->rbcp->Get_Bootservers()->at(i).Ident);

				offset += sizeof client->dhcp->rbcp->Get_Bootservers()->at(i).Ident;

				ipcount = static_cast<_BYTE>(client->dhcp->rbcp->Get_Bootservers()->at(i).Addresses.size());

				memcpy(&serverbuffer[offset], &ipcount, sizeof ipcount);
				offset += sizeof ipcount;

				for (const _IPADDR& address : client->dhcp->rbcp->Get_Bootservers()->at(i).Addresses)
				{
					memcpy(&serverbuffer[offset], &address, sizeof address);
					offset += sizeof address;
				}
			}

			client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_BOOT_SERVERS),
				static_cast<_BYTE>(offset), serverbuffer);

			delete[] serverbuffer;
			serverbuffer = nullptr;
		}

		template<class S>
		void Handle_TFTP_RRQ(ServiceType* type, Server<S>* server, Client<S>* client, const std::string& rootDir, Packet* packet)
		{
			client->tftp = new TFTP_CLIENT();
			client->tftp->SetFilename(Combine(rootDir, std::string(packet->Get_TFTPOption("File").Value)));

			if (!FileExist(client->tftp->GetFilename()))
			{
				client->tftp->Set_State(TFTP_ERROR);


				printf("[E] File not found: %s\n", client->tftp->GetFilename().c_str());

				client->response = new Packet(type, static_cast<_SIZET>((client->tftp->
					GetFilename().size() + 1) + 4), Packet_OPCode::TFTP_ERR);

				client->response->Write(static_cast<_USHORT>(BS16(5)), 0);
				client->response->Write(static_cast<_USHORT>(BS16(1)), 2);

				client->response->Write(client->tftp->GetFilename().c_str(),
					client->tftp->GetFilename().size(), 4);

				client->response->Commit();

				int retval = server->Send(client);

				if (retval == SOCKET_ERROR)
				{
					printf("[E] Error while sending %s Packet!\n", client->Get_TypeString().c_str());

				}

				client->tftp->Set_State(TFTP_ERROR);

				delete client->response;
				client->response = nullptr;

				return;
			}

			printf("[I] File requested: %s\n", client->tftp->GetFilename().c_str());

			if (client->tftp->OpenFile(client->tftp->GetFilename()))
			{
				client->response = new Packet(type, 1024, Packet_OPCode::TFTP_OACK);

				if (packet->Has_TFTPOption("tsize"))
					client->response->Add_TFTPOption(TFTP_Option("tsize", Functions::AsString(client->tftp->GetBytesToRead())));

				if (packet->Has_TFTPOption("blksize"))
				{
					client->tftp->SetBlockSize(Functions::AsUSHORT(packet->Get_TFTPOption("blksize").Value));
				}

				client->response->Add_TFTPOption(TFTP_Option("blksize", Functions::AsString(client->tftp->GetBlockSize())));

				client->response->Commit();
				int retval = server->Send(client);
				if (retval == SOCKET_ERROR)
				{
					printf("[E] Error while sending %s Packet!\n", client->Get_TypeString().c_str());
					client->tftp->Set_State(TFTP_ERROR);
					return;
				}

				delete client->response;
				client->response = nullptr;

				client->tftp->Set_State(TFTP_DOWNLOAD);
				return;
			}

			client->tftp->Set_State(TFTP_ERROR);

		}

		template<class S>
		void Handle_TFTP_ERR(ServiceType * type, Server<S> * server, Client<S> * client, Packet * packet)
		{
			_SIZET len = packet->get_Length() - 3;
			char* _message = new char[len];
			ClearBuffer(_message, len);
			packet->Read(_message, len, 4);

			_USHORT errcode = 1;
			packet->Read(&errcode, sizeof errcode, 2);
			print_Error(_message);

			delete[] _message;
			_message = nullptr;

			client->tftp->CloseFile();

			if (errcode != 0)
				client->tftp->Set_State(TFTP_ERROR);
		}

		template<class S>
		void Handle_TFTP_ACK(ServiceType * type, Server<S> * server, Client<S> * client, Packet * packet)
		{
			if (client->tftp->Get_State() != TFTP_DOWNLOAD)
			{
				client->tftp->Set_State(TFTP_ERROR);
				return;
			}

			_USHORT blk = BS16(client->tftp->GetCurrentBlock());
			bool isInSync = memcmp(&packet->Get_Buffer()[2], &blk, sizeof blk) == 0;

			if (!isInSync)
			{
				client->tftp->Set_State(CLIENTSTATE::TFTP_ERROR);
				print_Error("[W] Client got out of Sync!");
				return;
			}

			if (packet->get_Length() > 4)
			{
				_BYTE newWinSize = 1;

				packet->Read(&newWinSize, sizeof newWinSize, 4);
				client->tftp->SetWindowSize(newWinSize);
			}

			_SIZET chunk = 0;

			for (_BYTE i = 0; i < client->tftp->GetWindowSize(); i++)
			{
				chunk = static_cast<_SIZET>(client->tftp->GetBlockSize()) < client->tftp->GetBytesToRead() - client->tftp->GetBytesRead() ?
					static_cast<_SIZET>(client->tftp->GetBlockSize()) : (client->tftp->GetBytesToRead() - client->tftp->GetBytesRead());

				client->tftp->FileSeek();
				client->tftp->SetCurrentBlock(client->tftp->GetCurrentBlock() + 1);

				client->response = new Packet(type, static_cast<_SIZET>(4 + chunk), Packet_OPCode::TFTP_DAT);
				client->response->Set_Block(client->tftp->GetCurrentBlock());

				client->tftp->SetBytesRead(static_cast<long>(fread(&client->response->Get_Buffer()[4], 1, chunk, client->tftp->Get_FileHandle())));
				client->response->Commit();

				server->Send(client);

				delete client->response;
				client->response = nullptr;

				if (client->tftp->GetBytesRead() == client->tftp->GetBytesToRead())
				{
					client->tftp->Set_State(TFTP_DONE);
					printf("[I] (%s): Transfer completed!\n", client->Get_ID().c_str());
					client->tftp->CloseFile();
					break;
				}
			}
		}
	}
}
