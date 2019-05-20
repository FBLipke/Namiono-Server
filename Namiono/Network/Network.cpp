/*
 * Network.cpp
 *
 *  Created on: 18.02.2019
 *      Author: lipkegu
 */
#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Network
	{
		_ULONG requestId = 0;
		_USHORT id = 1;
		std::vector<std::thread> servers;
		std::vector<BootServerEntry> Serverlist;
		void add_server(_IPADDR address, const std::string& tftp_rootDir)
		{
			printf("[I] Listening on %s\n", Functions::AddressStr(address).c_str());

			servers.emplace_back(Namiono::Network::__serverThread<sockaddr_in>,
				DHCP_SERVER, AF_INET, address, 67, tftp_rootDir, Handle_Request<sockaddr_in>);

			servers.emplace_back(Namiono::Network::__serverThread<sockaddr_in>,
				BINL_SERVER, AF_INET, address, 4011, tftp_rootDir, Handle_Request<sockaddr_in>);

			servers.emplace_back(Namiono::Network::__serverThread<sockaddr_in>,
				TFTP_SERVER, AF_INET, address, 69, tftp_rootDir, Handle_Request<sockaddr_in>);
		}

		void Get_Interface_Addresses(const std::string& rootDir, std::function<void(_IPADDR, const std::string& rootDir)> cb)
		{
			std::vector<_IPADDR> addresses;
#ifdef _WIN32
			PIP_ADAPTER_INFO pAdapterInfo = NULL;
			PIP_ADAPTER_INFO pAdapter = NULL;
			_ULONG ulOutBufLen = 0;

			if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
			{
				free(pAdapterInfo);
				pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
				if (pAdapterInfo == NULL)
					return;
			}

			if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
			{
				pAdapter = pAdapterInfo;
				while (pAdapter)
				{
					_IPADDR address = inet_addr(pAdapter->IpAddressList.IpAddress.String);
					if (address != 0)
						addresses.emplace_back(address);

					cb(address, rootDir);

					pAdapter = pAdapter->Next;
				}
			}

			if (pAdapterInfo)
				free(pAdapterInfo);
#else
			struct ifaddrs* ifap, * ifa;
			struct sockaddr_in* sa;

			getifaddrs(&ifap);
			for (ifa = ifap; ifa; ifa = ifa->ifa_next)
			{
				if (ifa->ifa_addr->sa_family == AF_INET)
				{
					sa = (struct sockaddr_in*) ifa->ifa_addr;
					_IPADDR address = sa->sin_addr.s_addr;
					addresses.emplace_back(address);
					cb(address, rootDir);
				}
			}

			freeifaddrs(ifap);
#endif
			Serverlist.emplace_back(1, Get_Hostname(), addresses, "");
			addresses.clear();
		}


		void Bootstrap_Network(const std::string & tftp_rootDir)
		{
			printf("[I] Starting network...\n");

#ifdef _WIN32
			if (!Init_Winsock(2, 0))
			{
				printf("[E] Error: %s\n", strerror(WSAGetLastError()));
				return;
			}
#endif

			Get_Interface_Addresses(tftp_rootDir, add_server);

			for (std::thread& serverThread : servers)
				serverThread.join();

#ifdef _WIN32
			if (!Close_Winsock())
			{
				printf("[E] Closing Winsock - Error %d\n", WSAGetLastError());
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
				if (!packet->Has_DHCPOption(60))
					return;

				if (Functions::Compare(packet->Get_DHCPOption(60).Value, "PXEClient", 9))
					client->dhcp->Set_Vendor(PXEClient);
				else
				{
					if (Functions::Compare(packet->Get_DHCPOption(60).Value, "PXEServer", 9))
						client->dhcp->Set_Vendor(PXEServer);
					else
					{
						if (Functions::Compare(packet->Get_DHCPOption(60).Value, "AAPLBSDPC", 9))
							client->dhcp->Set_Vendor(APPLEBSDP);
						else
							return;
					}
				}

				client->dhcp->SetIsWDSRequest(packet->Has_DHCPOption(250));

				switch (static_cast<DHCP_MSGTYPE>(packet->Get_DHCPOption(53)
					.Get_Value_As_Byte()))
				{
				case DHCP_MSGTYPE::DISCOVER:
					Handle_DHCP_Discover(type, server, client, packet);
					break;
				case DHCP_MSGTYPE::INFORM:
				case DHCP_MSGTYPE::REQUEST:
					Handle_DHCP_Request(type, server, client, packet);
					break;
				case DHCP_MSGTYPE::RELEASE:
					server->Remove_Client(client);
					break;
				}
				if (client->dhcp->Get_State() == CLIENTSTATE::DHCP_DONE ||
					client->dhcp->Get_State() == CLIENTSTATE::DHCP_ABORT)
					server->Remove_Client(client);
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
					server->Remove_Client(client);
				break;
			}
		}

		template<class S>
		void Handle_IPXE_Options(Server<S> * server, Client<S> * client, Packet * response)
		{
			std::vector<DHCP_Option> options;
			response->Get_DHCPOption(175).Get_SubOptions(options);

			for (DHCP_Option& option : options)
			{
				IPXE_Options opt = static_cast<IPXE_Options>(option.Option);

				switch (opt)
				{
				case IPXE_USERNAME:
					client->dhcp->ipxe->Set_Username(option.Get_Value_As_String());
					break;
				case IPXE_PASSWORD:
					client->dhcp->ipxe->Set_Password(option.Get_Value_As_String());
					break;
				}
			}
		}

		template<class S>
		void Handle_WDS_Options(Server<S> * server, Client<S> * client, Packet * response)
		{

			// WDS Option -> Used by WDSNBP
			std::vector<DHCP_Option>* wdsOptions = new std::vector<DHCP_Option>();
			wdsOptions->emplace_back(WDSBP_OPT_NEXT_ACTION, static_cast<_BYTE>(client->dhcp->wds->GetNextAction()));
			wdsOptions->emplace_back(WDSBP_OPT_REQUEST_ID, static_cast<_ULONG>(BS32(client->dhcp->wds->GetRequestID())));

			wdsOptions->emplace_back(WDSBP_OPT_POLL_INTERVAL, static_cast<_USHORT>(BS16(client->dhcp->wds->GetPollInterval())));
			wdsOptions->emplace_back(WDSBP_OPT_POLL_RETRY_COUNT, static_cast<_USHORT>(BS16(client->dhcp->wds->GetRetryCount())));

			client->dhcp->wds->SetReferralServer(server->Get_IPAddress());
			if (client->dhcp->wds->GetNextAction() == REFERRAL)
			{
				wdsOptions->emplace_back(WDSBP_OPT_REFERRAL_SERVER, client->dhcp->wds->GetReferalServer());
				wdsOptions->emplace_back(WDSBP_OPT_ALLOW_SERVER_SELECTION, static_cast<_BYTE>(1));
			}

			wdsOptions->emplace_back(WDSBP_OPT_ACTION_DONE, static_cast<_BYTE>(client->dhcp->wds->GetActionDone()));

			if (client->dhcp->wds->GetWDSMessage().size() != 0)
				wdsOptions->emplace_back(WDSBP_OPT_MESSAGE, client->dhcp->wds->GetWDSMessage());

			response->Add_DHCPOption(DHCP_Option(250, *wdsOptions));
			wdsOptions->clear();
			delete wdsOptions;

		}

		template<class S>
		void Handle_DHCP_Discover(ServiceType * type, Server<S> * server, Client<S> * client, Packet * packet)
		{
			client->dhcp->Set_State(DHCP_INIT);

			std::vector<DHCP_Option> vendorOpts; // Move this into the Client scope!
			if (SETTINGS.PXEBOOTMENUE == 1)
			{
				Create_BootServer_List(vendorOpts);
				GenerateBootMenue(client, &vendorOpts);
				vendorOpts.emplace_back(static_cast<_BYTE>
					(PXE_DISCOVERY_CONTROL), static_cast<_BYTE>(SETTINGS.DISCOVERY_MODE));

				vendorOpts.emplace_back(static_cast<_BYTE>(PXE_DISCOVERY_MCADDR),
					static_cast<_IPADDR>(inet_addr(SETTINGS.DISCOVERY_ADDR.c_str())));

				vendorOpts.emplace_back(static_cast<_BYTE>(PXE_MTFTP_IP_ADDR),
					static_cast<_IPADDR>(inet_addr(SETTINGS.DISCOVERY_ADDR.c_str())));

				vendorOpts.emplace_back(static_cast<_BYTE>(PXE_MTFTP_SERVER_PORT), SETTINGS.MTFTP_SPORT);
				vendorOpts.emplace_back(static_cast<_BYTE>(PXE_MTFTP_CLIENT_PORT), SETTINGS.MTFTP_CPORT);

				/* LCM related Options */
				vendorOpts.emplace_back(static_cast<_BYTE>(PXE_LCM_DOMAIN), SETTINGS.NBDOMAIN);
				vendorOpts.emplace_back(static_cast<_BYTE>(PXE_LCM_SERVER), server->Get_ServerName());
				vendorOpts.emplace_back(static_cast<_BYTE>(PXE_LCM_DISCOVERY), static_cast<_BYTE>(1));
				vendorOpts.emplace_back(static_cast<_BYTE>(PXE_LCM_CONFIGURED), static_cast<_BYTE>(1));
				vendorOpts.emplace_back(static_cast<_BYTE>(PXE_LCM_VERSION), BS32(static_cast<_ULONG>(1)));
				vendorOpts.emplace_back(static_cast<_BYTE>(PXE_LCM_SERIALNO), std::string("Namiono - Server 0.5"));

			}

			Packet* response = new Packet(*packet, 1024, DHCP_MSGTYPE::OFFER);
			if (vendorOpts.size() != 0)
				response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(43), vendorOpts));

			response->set_filename("Boot/x86/wdsnbp.com");

			response->Add_DHCPOption(DHCP_Option(53, static_cast<_BYTE>(OFFER)));
			response->Add_DHCPOption(DHCP_Option(54, server->Get_IPAddress()));
			response->Add_DHCPOption(DHCP_Option(60, client->dhcp->Get_VendorString()));
			Handle_WDS_Options(server, client, response);

			response->Commit();

			// Sometimes we responding too fast, so wait here... 1 ms;
			Sleep(static_cast<BYTE>((1 + SETTINGS.SUBNETDELAY)));

			int retval = server->Send(client, response);
			if (retval == -1)
				printf("Error while sending %s packet! (%s)\n",
					client->Get_TypeString().c_str(), strerror(errno));

			delete response;
			client->dhcp->Set_State(DHCP_DONE);
		}

		template<class S>
		void Handle_DHCP_Request(ServiceType * type, Server<S> * server, Client<S> * client, Packet * packet)
		{
			Packet* response = new Packet(*packet, 1024, DHCP_MSGTYPE::ACK);
			response->Add_DHCPOption(DHCP_Option(54, server->Get_IPAddress()));
			response->Add_DHCPOption(DHCP_Option(66, server->Get_ServerName()));
			response->Add_DHCPOption(DHCP_Option(150, server->Get_IPAddress()));
			response->set_servername(server->Get_ServerName());
			response->set_nextIP(server->Get_IPAddress());
			response->set_filename("Boot/x86/wdsnbp.com");

			std::vector<DHCP_Option> vendorOpts; // Move this into the Client scope!
			if (packet->Has_DHCPOption(77) && packet->Has_DHCPOption(175))
			{
				if (Functions::Compare(packet->Get_DHCPOption(77)
					.Get_Value_As_String().c_str(), "iPXE", 4) ||
					Functions::Compare(packet->Get_DHCPOption(77)
						.Get_Value_As_String().c_str(), "gPXE", 4))
				{
					client->dhcp->SetIsIPXERequest(true);
					Handle_IPXE_Options(server, client, packet);
				}
			}

			if (packet->Has_DHCPOption(250) && client->dhcp->GetIsWDSRequest())
			{
				std::vector<DHCP_Option> options;
				packet->Get_DHCPOption(250).Get_SubOptions(options);

				for (DHCP_Option& opt : options)
					if (opt.Option == static_cast<_BYTE>(WDSBP_OPT_ARCHITECTURE))
					{
						DHCP_ARCH arch = INTEL_X86;

						memcpy(&arch, &opt.Value[0], sizeof(_USHORT));
						client->dhcp->SetArchitecture(arch);
						response->set_filename(client->dhcp->GetBootfile());
					}

				if (client->dhcp->GetIsWDSRequest())
				{
					requestId++;
					response->set_filename(Get_Bootfile_Path(client) + "pxeboot.n12");
					response->Add_DHCPOption(DHCP_Option(252, Get_Bootfile_Path(client) + "default.bcd"));

					client->dhcp->wds->SetRequestID(requestId);
					client->dhcp->Set_State(client->dhcp->wds->GetActionDone() == 0 ? DHCP_WAITING : DHCP_DONE);

				}

				Handle_WDS_Options(server, client, response);

				if (client->dhcp->GetIsWDSRequest())
					if (client->dhcp->wds->GetActionDone() == 0)
						return;
			}

			if (packet->Has_DHCPOption(43))
			{
				std::vector<DHCP_Option> options;
				packet->Get_DHCPOption(43).Get_SubOptions(options);

				// Get the selected Server...
				for (DHCP_Option& opt : options)
					if (opt.Option == static_cast<_BYTE>(PXE_BOOT_ITEM))
					{
						RBCP_LAYER layer = RBCP_LAYER::Bootfile;
						_USHORT type = 0;

						memcpy(&type, &opt.Value[0], sizeof(_USHORT));
						if (type != 0)
							client->dhcp->rbcp->Set_Item(type);

						memcpy(&layer, &opt.Value[2], sizeof(_USHORT));
						if (layer != 0)
							client->dhcp->rbcp->Set_Layer(layer);
					}
			}

			if (client->dhcp->rbcp->Get_Item() != 0)
			{
				char item[4];
				ClearBuffer(item, sizeof item);
				_USHORT type = client->dhcp->rbcp->Get_Item();
				RBCP_LAYER layer = client->dhcp->rbcp->Get_Layer();

				memcpy(&item[0], &type, sizeof(_USHORT));
				memcpy(&item[2], &layer, sizeof(_USHORT));

				vendorOpts.emplace_back(static_cast<_BYTE>(PXE_BOOT_ITEM), static_cast<_BYTE>(4), item);
				client->dhcp->SetNextServer(server->Get_IPAddress());
				
			}

			if (vendorOpts.size() != 0)
				response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(43), vendorOpts));

			response->Add_DHCPOption(DHCP_Option(53, static_cast<_BYTE>(ACK)));
			response->Add_DHCPOption(DHCP_Option(60, client->dhcp->Get_VendorString()));

			response->Commit();

			// Sometimes we responding too fast, so wait here... 1 ms;
			Sleep(static_cast<BYTE>((1 + SETTINGS.SUBNETDELAY)));

			int retval = server->Send(client, response);
			if (retval == SOCKET_ERROR)
				printf("Error while sending %s packet! (%s)\n", client->Get_TypeString().c_str(), strerror(errno));

			delete response;
		}

		template<class S>
		void GenerateBootMenue(Client<S> * client, std::vector<DHCP_Option> * vendorOpts)
		{
			_SIZET offset = 0;
			char menubuffer[1024];

			ClearBuffer(menubuffer, sizeof menubuffer);

			/* id */
			_USHORT id = 0;
			memcpy(&menubuffer[offset], &id, sizeof id);
			offset += sizeof id;

			/* desc len */
			_BYTE length = static_cast<_BYTE>(strlen("Local Boot"));
			memcpy(&menubuffer[offset], &length, sizeof length);
			offset += sizeof length;

			/* desc */
			memcpy(&menubuffer[offset], "Local Boot", length);
			offset += length;
			for (BootServerEntry& entry : Serverlist)
			{
				if (entry.Description.size() == 0)
					continue;

				/* id */
				id = entry.Ident;
				memcpy(&menubuffer[offset], &id, sizeof id);
				offset += sizeof id;

				/* desc len */
				length = static_cast<_BYTE>(strlen(entry.Description.c_str()));
				memcpy(&menubuffer[offset], &length, sizeof length);
				offset += sizeof length;

				/* desc */
				memcpy(&menubuffer[offset], entry.Description.c_str(), length);
				offset += length;
			}

			vendorOpts->emplace_back(static_cast<_BYTE>(PXE_BOOT_MENU),
				static_cast<_BYTE>(offset), menubuffer);

			/* Menue prompt */
			_BYTE timeout = SETTINGS.PXEPROMPTTIMEOUT;
			const std::string prompt = timeout != static_cast<_BYTE>(0xff) ?
				SETTINGS.PXEPROMP : std::string("Select Server...");

			char* promptbuffer = new char[prompt.size() + 1];
			ClearBuffer(menubuffer, prompt.size() + 1);
			offset = 0;

			memcpy(&promptbuffer[offset], &timeout, sizeof timeout);
			offset += sizeof timeout;

			memcpy(&promptbuffer[offset], prompt.c_str(),
				static_cast<_SIZET>(prompt.size()));

			offset += static_cast<_SIZET>(prompt.size());

			vendorOpts->emplace_back(static_cast<_BYTE>(PXE_MENU_PROMPT),
				static_cast<_BYTE>(offset), promptbuffer);

			delete[] promptbuffer;
		}

		void Create_BootServer_List(std::vector<DHCP_Option> & vendorOps)
		{
			_BYTE offset = 0;
			char* serverbuffer = new char[1024];
			ClearBuffer(serverbuffer, 1024);

			for (BootServerEntry& entry : Serverlist)
			{
				if (entry.Addresses.size() == 0 || entry.Description.size() == 0)
					continue;

				memcpy(&serverbuffer[offset], &entry.Ident, sizeof entry.Ident);
				offset += sizeof entry.Ident;

				_BYTE ipcount = static_cast<_BYTE>(entry.Addresses.size());

				memcpy(&serverbuffer[offset], &ipcount, sizeof ipcount);
				offset += sizeof ipcount;

				for (_IPADDR& address : entry.Addresses)
				{
					memcpy(&serverbuffer[offset], &address, sizeof address);
					offset += sizeof address;
				}
			}

			vendorOps.emplace_back(static_cast<_BYTE>(PXE_BOOT_SERVERS),
				static_cast<_BYTE>(offset), serverbuffer);

			delete[] serverbuffer;
		}

		template<class S>
		void Handle_TFTP_RRQ(ServiceType * type, Server<S> * server,
			Client<S> * client, const std::string & rootDir, Packet * packet)
		{
			using namespace Namiono::Environemnt::Filesystem;
			client->tftp->SetFilename(Combine(rootDir, std::string(packet->Get_TFTPOption("File").Value)));

			if (!FileExist(client->tftp->GetFilename().c_str()))
			{
				client->tftp->Set_State(TFTP_ERROR);
				printf("[E] File not found: %s\n", client->tftp->GetFilename().c_str());

				Packet* response = new Packet(static_cast<_SIZET>((client->tftp->
					GetFilename().size() + 1) + 4), Packet_OPCode::TFTP_ERR);
				
				response->Write(static_cast<_USHORT>(BS16(5)), 0);
				response->Write(static_cast<_USHORT>(BS16(1)), 2);

				response->Write(client->tftp->GetFilename().c_str(),
					client->tftp->GetFilename().size(), 4);

				response->Commit();

				int retval = server->Send(client, response);
				if (retval == SOCKET_ERROR)
				{
					printf("[E] Error while sending %s Packet!\n", client->Get_TypeString().c_str());
				}

				delete response;
				return;

			}

			if (client->tftp->OpenFile(client->tftp->GetFilename()))
			{

				printf("[I] (%s): File requested: %s\n", client->Get_ID().c_str(), client->tftp->GetFilename().c_str());

				Packet* response = new Packet(1024, Packet_OPCode::TFTP_OACK);

				if (packet->Has_TFTPOption("tsize"))
					response->Add_TFTPOption(TFTP_Option("tsize", Functions::AsString(client->tftp->GetBytesToRead())));

				if (packet->Has_TFTPOption("blksize"))
				{
					char blksize[6];
					ClearBuffer(blksize, sizeof blksize);
					memcpy(&blksize, packet->Get_TFTPOption("blksize").Value, sizeof blksize);
					client->tftp->SetBlockSize(static_cast<_USHORT>(atoi(blksize)));
				}

				response->Add_TFTPOption(TFTP_Option("blksize", Functions::AsString(client->tftp->GetBlockSize())));
				/*
							if (packet->Has_TFTPOption("windowsize"))
							{
								char winsize[6];
								ClearBuffer(winsize, 6);
								memcpy(&winsize, packet->Get_TFTPOption("windowsize").Value, sizeof winsize);
								client->tftp->SetWindowSize(static_cast<_USHORT>(atoi(winsize)));
								response->Add_TFTPOption(TFTP_Option("windowsize", Functions::AsString(client->tftp->GetWindowSize())));

								printf("[D] Using Windowsize: %d\n", client->tftp->GetWindowSize());
							}

							if (packet->Has_TFTPOption("msftwindow"))
							{
								char mswinsize[6];
								ClearBuffer(mswinsize, 6);
								memcpy(&mswinsize, packet->Get_TFTPOption("msftwindow").Value, sizeof mswinsize);
								// client->tftp->SetMSFTWindow(atoi(mswinsize));
								response->Add_TFTPOption(TFTP_Option("msftwindow", Functions::AsString(client->tftp->GetMSFTWindow())));
								printf("[D] Using MSFT Window: %d\n", client->tftp->GetMSFTWindow());
							}
							*/
				response->Commit();
				int retval = server->Send(client, response);
				if (retval == SOCKET_ERROR)
				{
					printf("[E] Error while sending %s Packet!\n", client->Get_TypeString().c_str());
				}

				delete response;
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
			memcpy(_message, &packet->Get_Buffer()[4], len);

			_USHORT errcode = 1;
			memcpy(&errcode, &packet->Get_Buffer()[2], sizeof errcode);
			print_Error(_message);
			delete[] _message;

			client->tftp->CloseFile();

			if (errcode != 0)
				client->tftp->Set_State(TFTP_ERROR);
		}

		template<class S>
		void Handle_TFTP_ACK(ServiceType* type, Server<S>* server, Client<S>* client, Packet* packet)
		{
			if (client->tftp->Get_State() != TFTP_DOWNLOAD)
			{
				client->tftp->Set_State(TFTP_ERROR);
				return;
			}

			if (client->tftp->GetCurrentBlock() != packet->Get_Block())
			{
				client->tftp->Set_State(TFTP_ERROR);
				print_Error("[W] Client got out of Sync!");
				return;
			}

			if (packet->get_Length() > 4)
			{
				// Clients can send new window size via ACK!
				printf("[I] (%s): Windowsize of %d suggested!\n", client->Get_ID().c_str(), packet->Get_Buffer()[4]);
				client->tftp->SetWindowSize(static_cast<_USHORT>(packet->Get_Buffer()[4]));
			}

			for (_USHORT i = 0; i < client->tftp->GetWindowSize(); i++)
			{
				_SIZET chunk = static_cast<_SIZET>(client->tftp->GetBlockSize()) < client->tftp->GetBytesToRead() - client->tftp->GetBytesRead() ?
					static_cast<_SIZET>(client->tftp->GetBlockSize()) : (client->tftp->GetBytesToRead() - client->tftp->GetBytesRead());

				client->tftp->FileSeek();
				client->tftp->SetCurrentBlock(client->tftp->GetCurrentBlock() + 1);
				
				Packet* response = new Packet(static_cast<_SIZET>(4 + chunk), Packet_OPCode::TFTP_DAT);
				response->Set_Block(client->tftp->GetCurrentBlock());

				client->tftp->SetBytesRead(static_cast<long>(fread(&response->Get_Buffer()[4], 1, chunk, client->tftp->Get_FileHandle())));
				response->Commit();
				int retval = server->Send(client, response);
				delete response;

				if (retval == SOCKET_ERROR)
					print_Error(strerror(errno), errno);

				if (client->tftp->GetBytesRead() == client->tftp->GetBytesToRead())
				{
					client->tftp->Set_State(TFTP_DONE);
					printf("[I] (%s): Transfer completed!\n", client->Get_ID().c_str());
					client->tftp->CloseFile();
					break;
				}
			}
		}

		template<class S>
		std::string Get_Bootfile_Path(Client<S>* client)
		{
			switch (BS16(client->dhcp->GetArchitecture()))
			{
			case INTEL_X86:
				return std::string("Boot\\x86\\");
			case INTEL_IA32X64:
				return std::string("Boot\\x64\\");
			case EFI_X86X64:
			case EFI_BC:
				return std::string("Boot\\efi\\");
			default:
				return std::string("Boot\\x86\\");
			}
		}
	}
}
