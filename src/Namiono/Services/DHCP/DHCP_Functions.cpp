#include <Namiono/Namiono.h>

DHCP_Functions::DHCP_Functions()
{
}

DHCP_Functions::~DHCP_Functions()
{
}

void DHCP_Functions::Generate_Bootmenu_From_ServerList(SETTINGS* settings, std::vector<BootServerEntry>* serverlist, Client* client)
{
	_SIZET offset = 0;
	char menubuffer[1024];

	ClearBuffer(menubuffer, sizeof menubuffer);

	/* id */
	_USHORT id = 0;
	_USHORT _LE_ID = ntohs(id);
	memcpy(&menubuffer[offset], &id, sizeof id);
	offset += sizeof id;

	/* desc len */
	_BYTE length = static_cast<_BYTE>(settings->PXEHDDDESC.size());
	memcpy(&menubuffer[offset], &length, sizeof length);
	offset += sizeof length;

	/* desc */
	memcpy(&menubuffer[offset], settings->PXEHDDDESC.c_str(), length);
	offset += length;

	for (_USHORT i = 0; i < serverlist->size(); i++)
	{
		if (serverlist->at(i).Description.size() == 0)
			continue;

		/* type */
		id = serverlist->at(i).Type;
		_LE_ID = id;
		memcpy(&menubuffer[offset], &_LE_ID, sizeof _LE_ID);
		offset += sizeof _LE_ID;

		/* desc len */
		length = static_cast<_BYTE>(strlen(serverlist->at(i).Description.c_str()));
		memcpy(&menubuffer[offset], &length, sizeof length);
		offset += sizeof length;

		/* desc */
		memcpy(&menubuffer[offset], serverlist->at(i).Description.c_str(), length);
		offset += length;
	}

	client->Get_DHCP_Client()->Get_VendorOpts()->emplace_back(static_cast<_BYTE>(PXE_BOOT_MENU),
		static_cast<_BYTE>(offset), menubuffer);

	/* Menue prompt */
	_BYTE timeout = settings->PXEPROMPTTIMEOUT;
	const std::string prompt = timeout != static_cast<_BYTE>(0xff) ?
		settings->PXEPROMP : std::string("These servers (*) responded to your request...");

	char* promptbuffer = new char[prompt.size() + 1];
	ClearBuffer(promptbuffer, prompt.size() + 1);
	offset = 0;

	memcpy(&promptbuffer[offset], &timeout, sizeof timeout);
	offset += sizeof timeout;

	memcpy(&promptbuffer[offset], prompt.c_str(),
		static_cast<_SIZET>(prompt.size()));

	offset += static_cast<_SIZET>(prompt.size());

	client->Get_DHCP_Client()->Get_VendorOpts()->emplace_back(static_cast<_BYTE>(PXE_MENU_PROMPT),
		static_cast<_BYTE>(offset), promptbuffer);

	delete[] promptbuffer;
	promptbuffer = nullptr;
}

void DHCP_Functions::Create_BootServerList(std::vector<BootServerEntry>* serverlist, Client* client)
{
	if (serverlist->size() == 0)
		return;

	_BYTE offset = 0;
	char* serverbuffer = new char[1024];
	ClearBuffer(serverbuffer, 1024);

	for (_USHORT i = 0; i < serverlist->size(); i++)
	{
		if (serverlist->at(i).Addresses.size() == 0 ||
			serverlist->at(i).Description.size() == 0)
			continue;

		_USHORT id = serverlist->at(i).Type;
		_USHORT _LE_ID = ntohs(id);
		memcpy(&serverbuffer[offset], &_LE_ID, sizeof _LE_ID);
		offset += sizeof _LE_ID;

		_BYTE ipcount = static_cast<_BYTE>(serverlist->at(i).Addresses.size());

		memcpy(&serverbuffer[offset], &ipcount, sizeof ipcount);
		offset += sizeof ipcount;

		for (const _IPADDR& address : serverlist->at(i).Addresses)
		{
			memcpy(&serverbuffer[offset], &address, sizeof address);
			offset += sizeof address;
		}
	}

	client->Get_DHCP_Client()->Get_VendorOpts()->emplace_back(static_cast<_BYTE>(PXE_BOOT_SERVERS),
		static_cast<_BYTE>(offset), serverbuffer);

	delete[] serverbuffer;
	serverbuffer = nullptr;
}

void DHCP_Functions::Relay_Request_Packet(const _IPADDR& addr, const _USHORT& port,	const ServiceType& type, Server * server, _USHORT iface, Client * client)
{
	/* Client request */
	client->Get_DHCP_Client()->Set_State(CLIENTSTATE::DHCP_RELAY);
	client->Set_Relay_Hint(addr, port);

	/*
	Put the IP Address from the interface which received the request in the giaddr field...
	The Upstream Server will send this response back to this address in the giaddr field...

	We need also the giaddr to identify the interface... when receiving responses...
	*/

	Iface* iFace = server->Get_Interface(type, client->GetIncomingInterface());
	
	if (iFace == nullptr)
	{
		iFace = server->Get_Interface(type, iface);
		printf("[W] Client interface not found! => Using %d instead...\n", iFace->Get_Id());
	}

	client->response->set_relayIP(server->Get_Interface(type, iFace->Get_Id())->Get_IPAddress());
	DHCP_Functions::Handle_Relayed_Packet(type, server, iFace->Get_Id(), client->response);

	std::string mac = Functions::MacAsString(&client->response->Get_Buffer()[28]).c_str();

	/*
		Store the mac in our cache on the incoming interface, so that we can later
		identify the Interface from where we got the request.

		The Interface id is already stored in option 82-> [1] but it must not be the right one.
	*/

	if (!iFace->Has_ARPEntry(mac))
		iFace->Add_ARPEntry(mac);

	switch (client->Get_DHCP_Client()->Get_Vendor())
	{
	case PXEClient:
		client->response->Add_DHCPOption(DHCP_Option(54, 4, client->Get_Relay_Hint().sin_addr.s_addr));
		break;
	default:
		break;
	}

	client->response->set_flags(DHCP_FLAGS::Unicast);
	client->response->increase_hops(1);
	client->response->Commit();
	
	std::vector<Iface> _interfaces;
	server->Get_Interfaces(type, &_interfaces);

	for (Iface& _iface : _interfaces)
	{
		server->Send(type, _iface.Get_Id(), client);
	}
}

_USHORT DHCP_Functions::Handle_Relayed_Packet(const ServiceType& type, Server * server, _USHORT iface, Packet * packet)
{
	std::vector<DHCP_Option> relayOptions;
	_USHORT _interface = iface;
	
	if (packet->Has_DHCPOption(static_cast<_BYTE>(82)))
	{
		packet->Get_DHCPOption(82).Get_SubOptions(relayOptions);
		
		std::string remoteid = "";

		for (_SIZET i = 0; i < relayOptions.size(); i++)
		{
			if (relayOptions.at(i).Option == CircuitID)
			{
				_interface = relayOptions.at(i).Get_Value_As_USHORT();
				printf("[D] Interface: %d\n", _interface);
			}

			if (relayOptions.at(i).Option == RemoteID)
			{
				remoteid = relayOptions.at(i).Get_Value_As_String();
#if _DEBUG
				printf("[D] Remote ID: %s\n", remoteid.c_str());
#endif
			}
		}

		relayOptions.clear();
		packet->Remove_DHCPOption(static_cast<_BYTE>(82));

		_interface = server->Get_Interface_by_Address(
			packet->get_hwaddress(), type, packet->get_relayIP());
	}
	else
	{
		if (relayOptions.size() == 0)
		{
#if _DEBUG
			printf("[D] Adding Relay informations...\n");
#endif
			relayOptions.emplace_back(DHCP_Option(static_cast<_BYTE>(RemoteID),
				server->Get_Interface(type, iface)->Get_ServerName()));

			relayOptions.emplace_back(DHCP_Option(static_cast<_BYTE>(CircuitID),
				static_cast<_BYTE>(iface)));

			packet->Add_DHCPOption(DHCP_Option(82, relayOptions));
		}
	}

	return _interface;
}

void DHCP_Functions::Relay_Response_Packet(std::map<std::string, DHCP_RELAYSESSION>* relaySessions,
	const ServiceType& type, Server * server, _USHORT iface, Client * client, Packet* packet)
{
	

	if (relaySessions->find(client->Get_ID()) != relaySessions->end())
	{
		client->response->set_relayIP(relaySessions->at(client->Get_ID()).Get_RelayIP());
		
		switch (type)
		{
		case DHCP_SERVER:
			client->Set_Client_Hint(relaySessions->at(client->Get_ID()).Get_RemoteIP(), 67);
			break;
		case BINL_SERVER:
			client->Set_Client_Hint(relaySessions->at(client->Get_ID()).Get_RemoteIP(), 4011);
			break;
		default:
			break;
		}

		client->SetIncomingInterface(relaySessions->at(client->Get_ID()).Get_Interface());
		client->response->set_flags(DHCP_FLAGS::Unicast);
		relaySessions->erase(client->Get_ID());
	}

	switch (type)
	{
	case BINL_SERVER:
	case DHCP_SERVER:
		switch (client->Get_DHCP_Client()->Get_Vendor())
		{
		case PXEServer:
		case PXEClient:
			/*
				Before we respond to the client we should fixup the server in the Packet, So that clients
				talks to us instead to the upstream server. We leave all other options as they are and adding the
				responding server to the list of boot servers later, so that the client can also boot from the upstream server.
			*/

			client->response->set_nextIP(server->Get_Interface(type, client->GetIncomingInterface())->Get_IPAddress());
			client->response->set_servername(server->Get_Interface(type, client->GetIncomingInterface())->Get_ServerName());

			client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(54),
				static_cast<_ULONG>(server->Get_Interface(type, client->GetIncomingInterface())->Get_IPAddress())));

			printf("[I] Relaying PXE request...\n");
			break;
		default:
			printf("[I] Relaying DHCP request...\n");
			break;
		}
		break;
	default:
		break;
	}

	client->response->Commit();

	if (server->Get_Interface(type, client->GetIncomingInterface())->Get_ServiceType() == type)
		server->Send(type, client->GetIncomingInterface(), client);
}

void DHCP_Functions::Add_BootServer_To_ServerList(std::vector<BootServerEntry>* serverlist,
	Server* server, Client * client, const std::string& serverName, const std::string& bootfile)
{
	if (client->Get_Server_Hint().sin_addr.s_addr == 0 && serverlist->size() != 0)
	{
		return;
	}

	std::vector<_IPADDR> addrs;
	addrs.emplace_back(client->Get_Server_Hint().sin_addr.s_addr);

	std::string _name = serverName;

	if (_name.size() == 0)
		_name = Functions::AddressStr(client->Get_Server_Hint().sin_addr.s_addr);

	BootServerType bsType = BootServerType::PXEBootstrapServer;

	if (client->Get_DHCP_Client()->GetIsWDSRequest() || client->Get_DHCP_Client()->GetIsWDSResponse())
		bsType = BootServerType::WindowsNTBootServer;

	DHCP_Functions::Add_BootServer(serverlist, bsType, std::string("(*) ") + _name, addrs, bootfile);
}

bool DHCP_Functions::Has_BootServer(std::vector<BootServerEntry>* serverlist, const _USHORT& id)
{
	if (serverlist->size() == 0 || serverlist->size() < id)
		return false;

	BootServerEntry* entry = &serverlist->at(id - 1);

	return entry != nullptr;
}

void DHCP_Functions::Add_BootServer(std::vector<BootServerEntry>* serverlist, BootServerType bstype,
	const std::string& name, const std::vector <_IPADDR>& addresses, const std::string& bootfile)
{
	_USHORT id = static_cast<_USHORT>(serverlist->size() + 1);

	if (addresses.size() == 0)
	{
		printf("[E] Passed Bootserver %s without addresses!\n", name.c_str());
		return;
	}

	if (Has_BootServer(serverlist, id))
		return;

	serverlist->emplace_back(BootServerEntry(id, bstype, name, addresses, bootfile.size() == 0 ? "" : bootfile));
}

void DHCP_Functions::Handle_IPXE_Options(Server * server, _USHORT iface, Client * client, Packet * response)
{
	std::vector<DHCP_Option> options;
	response->Get_DHCPOption(static_cast<_BYTE>(175)).Get_SubOptions(options);

	for (_SIZET i = 0; i < options.size(); i++)
	{
		switch (static_cast<IPXE_Options>(options.at(i).Option))
		{
		case IPXE_USERNAME:
			client->Get_DHCP_Client()->Get_IPXEClient()->Set_Username(options.at(i).Get_Value_As_String());
			break;
		case IPXE_PASSWORD:
			client->Get_DHCP_Client()->Get_IPXEClient()->Set_Password(options.at(i).Get_Value_As_String());
			break;
		case IPXE_BUSID:
			break;
		}
	}
}

void DHCP_Functions::Handle_WDS_Options(const SETTINGS* settings, const ServiceType& type, Server* server, _USHORT iface, Client * client)
{
	std::vector<DHCP_Option>* wdsOptions = new std::vector<DHCP_Option>();

	wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_NEXT_ACTION), static_cast<_BYTE>(client->Get_DHCP_Client()->Get_WDSClient()->GetNextAction()));
	wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_REQUEST_ID), static_cast<_ULONG>(htonl(client->Get_DHCP_Client()->Get_WDSClient()->GetRequestID())));

	wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_POLL_INTERVAL), static_cast<_USHORT>(BS16(client->Get_DHCP_Client()->Get_WDSClient()->GetPollInterval())));
	wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_POLL_RETRY_COUNT), static_cast<_USHORT>(BS16(client->Get_DHCP_Client()->Get_WDSClient()->GetRetryCount())));

	if (settings->WDS_SERVERSELECTION == 1)
	{
		wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_ALLOW_SERVER_SELECTION), static_cast<_BYTE>(settings->WDS_SERVERSELECTION));
		wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_PXE_CLIENT_PROMPT), static_cast<_BYTE>(1));
	}
	
	if (client->Get_DHCP_Client()->Get_WDSClient()->GetNextAction() == REFERRAL)
		wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_REFERRAL_SERVER), client->Get_DHCP_Client()->Get_WDSClient()->GetReferalServer());

	wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_ACTION_DONE), static_cast<_BYTE>(client->Get_DHCP_Client()->Get_WDSClient()->GetActionDone()));

	if (client->Get_DHCP_Client()->Get_WDSClient()->GetNextAction() == APPROVAL && client->Get_DHCP_Client()->Get_WDSClient()->GetWDSMessage().size() != 0)
		wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_MESSAGE), client->Get_DHCP_Client()->Get_WDSClient()->GetWDSMessage());

	wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_END));
	client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(250), *wdsOptions));

	wdsOptions->clear();

	delete wdsOptions;
	wdsOptions = nullptr;
}
