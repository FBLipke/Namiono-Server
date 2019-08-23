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
		std::vector<Server> servers;
		std::vector<_IPADDR> addresses;
		std::vector<std::thread>listenThreads;

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
			servers.emplace_back(&addresses, tftp_rootDir, Handle_Request);

			for (_SIZET i = 0; i < servers.size(); i++)
			{
				servers.at(i).Init();
			}

			for (_SIZET i = 0; i < servers.size(); i++)
			{
				servers.at(i).Start();
			}

			for (_SIZET i = 0; i < servers.size(); i++)
			{
				servers.at(i).Listen(&listenThreads);
			}

			for (_SIZET i = 0; i < listenThreads.size(); i++)
			{
				listenThreads.at(i).join();
			}

			for (_SIZET i = 0; i < servers.size(); i++)
			{
				servers.at(i).Close();
			}

#ifdef _WIN32
			if (!Close_Winsock())
			{
				printf("[E] Closing Winsock - Error: %d\n", WSAGetLastError());
				return;
			}
#endif
		}

		void Handle_Request(ServiceType type, Server* server, int iface, Client* client, const std::string rootDir, Packet * packet)
		{
			switch (type)
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
										Relay_Response_Packet(&type, server, iface, client, packet);
										return;
									}
								}
								else
								{
									if (packet->get_opcode() == BOOTREQUEST)
									{
										Relay_Request_Packet(&type, server, iface, client, packet);
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
						Handle_DHCP_Discover(&type, server, iface, client, packet);
						break;
					case DHCP_MSGTYPE::INFORM:
					case DHCP_MSGTYPE::REQUEST:
						Handle_DHCP_Request(&type, server, iface, client, packet);
						break;
					}

					if (client->dhcp->Get_State() == CLIENTSTATE::DHCP_DONE ||
						client->dhcp->Get_State() == CLIENTSTATE::DHCP_ABORT)
						server->Remove_Client(client->Get_ID());
					break;
				case BINL_RQU:
					Handle_RIS_RQU(&type, server, iface, client, packet, rootDir);
					break;
				case BINL_NEG:
					Handle_RIS_NEG(&type, server, iface, client, packet);
					break;
				default:
					break;
				}

				break;
			case TFTP_SERVER:
				switch (static_cast<TFTP_OPCODE>(packet->Get_Opcode()))
				{
				case Packet_OPCode::TFTP_RRQ:
					Handle_TFTP_RRQ(&type, server, iface, client, rootDir, packet);
					break;
				case Packet_OPCode::TFTP_ACK:
					Handle_TFTP_ACK(&type, server, iface, client, packet);
					break;
				case Packet_OPCode::TFTP_ERR:
					Handle_TFTP_ERR(&type, server, iface, client, packet);
					break;
				}

				if (client->tftp->Get_State() == CLIENTSTATE::TFTP_DONE
					|| client->tftp->Get_State() == CLIENTSTATE::TFTP_ERROR)
					server->Remove_Client(client->Get_ID());
				break;
			}
		}

		void Handle_Relayed_Packet(Server* server, int iface, Packet* packet)
		{
			if (packet->Has_DHCPOption(static_cast<_BYTE>(82)))
			{

				/* For now remove them ... */
				//packet->Remove_DHCPOption(static_cast<_BYTE>(82));
			}
			else
			{
				std::vector<DHCP_Option> relayOptions;
				relayOptions.emplace_back(DHCP_Option(static_cast<_BYTE>(1), server->Get_Interface(iface)->Get_ServerName()));
				relayOptions.emplace_back(DHCP_Option(static_cast<_BYTE>(2), packet->get_xid()));
				
				packet->Add_DHCPOption(DHCP_Option(82, relayOptions));
			}
		}

		void Relay_Request_Packet(ServiceType * type, Server* server, int iface, Client* client, Packet * packet)
		{
			/* Client request */
			client->dhcp->Set_State(CLIENTSTATE::DHCP_RELAY);
			client->Set_Relay_Hint(inet_addr("10.20.0.1"), 67);
			Handle_Relayed_Packet(server, iface, packet);

			std::string mac = Functions::MacAsString(&packet->Get_Buffer()[28],
				static_cast<_SIZET>(packet->get_hwlength())).c_str();

			/*
				Store the mac in our cache on the incoming interface, so that we can later
				identify the Interface from where we got the request.
			*/

			if (!server->Get_Interface(iface)->Has_ARPEntry(mac))
				server->Get_Interface(iface)->Add_ARPEntry(mac);
			printf("[I] Forwarding BOOTREQUEST from %s (on Interface %d) to 10.20.0.1...\n", mac.c_str(), iface);
			
			/*
			Put the IP Address from the interface which received the request as giaddr field...
			The Upstream DHCP server will send his response Packet back to this giaddr address...

			We need also the giaddr to identify the interface... when receiving responses...
			*/
			
			packet->set_relayIP(server->Get_Interface(iface)->Get_IPAddress());
			packet->set_flags(DHCP_FLAGS::Unicast);
			packet->increase_hops(1);
			client->response = packet;
			client->response->Trim();

			server->Send(iface, client);
		}

		void Relay_Response_Packet(ServiceType * type, Server* server, int iface, Client* client, Packet * packet)
		{
			/* Server response */
			client->response = packet;
			client->Set_Client_Hint(INADDR_BROADCAST, 68);

			packet->set_flags(DHCP_FLAGS::Broadcast);
			Handle_Relayed_Packet(server, iface, packet);
			std::string mac = Functions::MacAsString(&packet->Get_Buffer()[28],
				static_cast<_SIZET>(packet->get_hwlength())).c_str();

			int _outgoing_iface = server->Get_Interface_by_Mac(mac);

			/* Send it out to the receiving Interface ... anyway ... */
			if (_outgoing_iface == -1)
				_outgoing_iface = iface;

			printf("[I] Forwarding BOOTREPLY to %s via Interface %d...\n", mac.c_str(), _outgoing_iface);

			client->response->Trim();

			server->Send(_outgoing_iface, client);
			client->dhcp->Set_State(CLIENTSTATE::DHCP_DONE);
		}

		void Handle_RIS_RQU(ServiceType * type, Server* server, int iface, Client* client, Packet * packet, const std::string& rootDir)
		{
			std::string oscfile = Combine(rootDir, Combine(std::string("OSChooser"), Combine(std::string("English"),
				packet->get_osc_filename(rootDir).c_str())));

			printf("[I] OSC File: %s\n", oscfile.c_str());
			printf("[I] OSC Client: %s\n", client->Get_ID().c_str());

			if (FileExist(oscfile))
			{
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

						client->response = new Packet(*type, osc_content.size() + 36, BINL_RSU);
						client->response->CopyFrom(*packet, 8, 8, 28);
						client->response->Write(osc_content.c_str(), osc_content.size(), 28);
						client->response->Commit();

						server->Send(iface, client);

						delete client->response;
						client->response = nullptr;


						fclose(fil);
					}
				}
			}
			else
			{
				print_Error("The File was not found!");

				/* Send an OSC "NOTFOUND message back to the client... */
			}
		}

		void Handle_RIS_NEG(ServiceType * type, Server* server, int iface, Client* client, Packet * packet)
		{
		}

		void Handle_RIS_AUT(ServiceType * type, Server* server, int iface, Client* client, Packet * packet)
		{
		}

		void Handle_IPXE_Options(Server* server, int iface, Client* client, Packet * response)
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

		void Handle_WDS_Options(Server* server, int iface, Client* client)
		{
			// WDS Option -> Used by WDSNBP
			std::vector<DHCP_Option>* wdsOptions = new std::vector<DHCP_Option>();
			wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_NEXT_ACTION), static_cast<_BYTE>(client->dhcp->wds->GetNextAction()));
			wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_REQUEST_ID), static_cast<_ULONG>(BS32(client->dhcp->wds->GetRequestID())));

			wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_POLL_INTERVAL), static_cast<_USHORT>(BS16(client->dhcp->wds->GetPollInterval())));
			wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_POLL_RETRY_COUNT), static_cast<_USHORT>(BS16(client->dhcp->wds->GetRetryCount())));

			if (client->dhcp->wds->GetNextAction() == REFERRAL)
			{
				client->dhcp->wds->SetReferralServer(server->Get_Interface(iface)->Get_IPAddress());
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

		int GenerateBootServers(Client* client)
		{
			client->dhcp->Set_State(DHCP_INIT);
			client->dhcp->rbcp->Clear_BootServers();
			if (addresses.size() != 0)
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

						for (_SIZET i = 0; i < _addrs.size(); i++)
						{
							addrs.emplace_back(inet_addr(_addrs.at(i).c_str()));
						}

						client->dhcp->rbcp->Add_BootServer(std::string(desc), addrs);
					}
				}

				fclose(fil);
			}

			return 0;
		}

		void Handle_DHCP_Discover(ServiceType * type, Server* server, int iface, Client* client, Packet * packet)
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

					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_DOMAIN), SETTINGS.NBDOMAIN);
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_SERVER), server->Get_Interface(iface)->Get_ServerName());
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_DISCOVERY), static_cast<_BYTE>(1));
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_CONFIGURED), static_cast<_BYTE>(1));
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_VERSION), BS32(static_cast<_UINT>(1)));
					client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_LCM_SERIALNO), std::string("Namiono - Server 0.5"));
				}

				client->response = new Packet(*type, *packet, 1024, DHCP_MSGTYPE::OFFER);
				if (client->dhcp->vendorOpts->size() != 0)
					client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(43),
						*client->dhcp->vendorOpts));

				client->response->set_servername(server->Get_Interface(iface)->Get_ServerName());
				client->response->set_nextIP(server->Get_Interface(iface)->Get_IPAddress());

				client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(53), static_cast<_BYTE>(OFFER)));
				client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(54), static_cast<_ULONG>(client->dhcp->GetNextServer())));
				client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(60), client->dhcp->Get_VendorString()));

				Handle_WDS_Options(server, iface, client);
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
		}

		void Handle_DHCP_Request(ServiceType * type, Server* server, int iface, Client* client, Packet * packet)
		{
			std::vector<DHCP_Option> options;
			std::string _serverName = "";
			_IPADDR _bootServer = 0;

			client->response = new Packet(*type, *packet, 1024, DHCP_MSGTYPE::ACK);
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
						Handle_IPXE_Options(server, iface, client, packet);
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

				Handle_WDS_Options(server, iface, client);

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

				_serverName = server->Get_Interface(iface)->Get_ServerName();
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

						if (server->Get_Interface(iface)->Get_Port() == 4011)
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

			server->Send(iface,client);

			delete client->response;
			client->response = nullptr;
		}

		void GenerateBootMenue(Client* client)
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

		void Create_BootServer_List(Client* client)
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

		void Handle_TFTP_RRQ(ServiceType* type, Server* server, int iface, Client* client, const std::string& rootDir, Packet* packet)
		{
			client->tftp = new TFTP_CLIENT();
			client->tftp->SetFilename(Combine(rootDir, std::string(packet->Get_TFTPOption("File").Value)));

			if (!FileExist(client->tftp->GetFilename()))
			{
				client->tftp->Set_State(TFTP_ERROR);


				printf("[E] File not found: %s\n", client->tftp->GetFilename().c_str());

				client->response = new Packet(*type, static_cast<_SIZET>((client->tftp->
					GetFilename().size() + 1) + 4), Packet_OPCode::TFTP_ERR);

				client->response->Write(static_cast<_USHORT>(BS16(5)), 0);
				client->response->Write(static_cast<_USHORT>(BS16(1)), 2);

				client->response->Write(client->tftp->GetFilename().c_str(),
					client->tftp->GetFilename().size(), 4);

				client->response->Commit();

				server->Send(iface, client);

				client->tftp->Set_State(TFTP_ERROR);

				delete client->response;
				client->response = nullptr;

				return;
			}

			printf("[I] File requested: %s\n", client->tftp->GetFilename().c_str());

			if (client->tftp->OpenFile(client->tftp->GetFilename()))
			{
				client->response = new Packet(*type, 1024, Packet_OPCode::TFTP_OACK);

				if (packet->Has_TFTPOption("tsize"))
					client->response->Add_TFTPOption(TFTP_Option("tsize", Functions::AsString(client->tftp->GetBytesToRead())));

				if (packet->Has_TFTPOption("blksize"))
				{
					client->tftp->SetBlockSize(Functions::AsUSHORT(packet->Get_TFTPOption("blksize").Value));
				}

				client->response->Add_TFTPOption(TFTP_Option("blksize", Functions::AsString(client->tftp->GetBlockSize())));

				client->response->Commit();
				server->Send(iface, client);
				
				delete client->response;
				client->response = nullptr;

				client->tftp->Set_State(TFTP_DOWNLOAD);
				return;
			}

			client->tftp->Set_State(TFTP_ERROR);

		}

		void Handle_TFTP_ERR(ServiceType * type, Server* server, int iface, Client* client, Packet * packet)
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

		void Handle_TFTP_ACK(ServiceType * type, Server* server, int iface, Client* client, Packet * packet)
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

				client->response = new Packet(*type, static_cast<_SIZET>(4 + chunk), Packet_OPCode::TFTP_DAT);
				client->response->Set_Block(client->tftp->GetCurrentBlock());

				client->tftp->SetBytesRead(static_cast<long>(fread(&client->response->Get_Buffer()[4], 1, chunk, client->tftp->Get_FileHandle())));
				client->response->Commit();

				server->Send(iface, client);

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
