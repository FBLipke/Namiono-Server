#include <Namiono/Namiono.h>

DHCP_Functions::DHCP_Functions()
{
}

DHCP_Functions::~DHCP_Functions()
{
}

void DHCP_Functions::Generate_Bootmenu_From_ServerList(SETTINGS* settings, std::vector<BootServerEntry>* serverlist, Client* client)
{
	if (serverlist->size() == 0)
		return;

	_SIZET offset = 0;
	char menubuffer[1024];

	ClearBuffer(menubuffer, sizeof menubuffer);

	/* id */
	_USHORT id = 0;
	_USHORT _LE_ID = LE16(id);
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

		/* id */
		id = serverlist->at(i).Ident;
		_LE_ID = LE16(id);
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
	_BYTE offset = 0;
	char* serverbuffer = new char[1024];
	ClearBuffer(serverbuffer, 1024);

	if (serverlist->size() == 0)
		return;

	for (_USHORT i = 0; i < serverlist->size(); i++)
	{
		if (serverlist->at(i).Addresses.size() == 0 ||
			serverlist->at(i).Description.size() == 0)
			continue;


		_USHORT id = serverlist->at(i).Ident;
		_USHORT _LE_ID = LE16(id);
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

void DHCP_Functions::Relay_Request_Packet(const _IPADDR& addr, const _USHORT& port,
	const ServiceType& type, Server * server, _INT32 iface, Client * client)
{
	/* Client request */
	client->Get_DHCP_Client()->Set_State(CLIENTSTATE::DHCP_RELAY);
	client->Set_Relay_Hint(addr, port);

	/*
	Put the IP Address from the interface which received the request in the giaddr field...
	The Upstream DHCP server will send this response back to this address in the giaddr field...

	We need also the giaddr to identify the interface... when receiving responses...
	*/

	client->response->set_relayIP(server->Get_Interface(type, iface)->Get_IPAddress());
	DHCP_Functions::Handle_Relayed_Packet(type, server, iface, client->response);

	std::string mac = Functions::MacAsString(&client->response->Get_Buffer()[28],
		static_cast<_SIZET>(client->response->get_hwlength())).c_str();

	/*
		Store the mac in our cache on the incoming interface, so that we can later
		identify the Interface from where we got the request.

		The Interface id is already stored in option 82-> [1] but it must not be the right one.
	*/

	if (!server->Get_Interface(type, iface)->Has_ARPEntry(mac))
		server->Get_Interface(type, iface)->Add_ARPEntry(mac);

	switch (client->Get_DHCP_Client()->Get_Vendor())
	{
	case PXEClient:
		client->response->Add_DHCPOption(DHCP_Option(54, client->Get_Relay_Hint().sin_addr.s_addr));
		break;
	default:
		break;
	}

	client->response->set_flags(DHCP_FLAGS::Unicast);
	client->response->increase_hops(1);
	client->response->Commit();

	for (Iface & iface : server->Get_Interfaces())
		server->Send(type, iface.Get_Id(), client);
}

_INT32 DHCP_Functions::Handle_Relayed_Packet(const ServiceType& type, Server * server, _INT32 iface, Packet * packet)
{
	std::vector<DHCP_Option> relayOptions;

	if (packet->Has_DHCPOption(static_cast<_BYTE>(82)))
	{
		packet->Get_DHCPOption(82).Get_SubOptions(relayOptions);
		_INT32 _interface = -1;
		std::string remoteid = "";

		for (_SIZET i = 0; i < relayOptions.size(); i++)
		{
			if (relayOptions.at(i).Option == CircuitID)
			{
				_interface = relayOptions.at(i).Get_Value_As_Byte();
			}

			if (relayOptions.at(i).Option == RemoteID)
			{
				remoteid = relayOptions.at(i).Get_Value_As_String();
			}
		}

		packet->Remove_DHCPOption(static_cast<_BYTE>(82));

		if (_interface == -1)
		{
			_interface = server->Get_Interface_by_Address(packet->get_hwaddress(), type, packet->get_relayIP());

			if (_interface == -1)
				return -1;
			else
				return _interface;
		}
		else
			return _interface;
	}
	else
	{
		relayOptions.emplace_back(DHCP_Option(static_cast<_BYTE>(RemoteID),
			server->Get_Interface(type, iface)->Get_ServerName()));

		relayOptions.emplace_back(DHCP_Option(static_cast<_BYTE>(CircuitID),
			static_cast<_BYTE>(iface)));

		packet->Add_DHCPOption(DHCP_Option(82, relayOptions));
	}

	return -1;
}

void DHCP_Functions::Relay_Response_Packet(std::map<std::string, DHCP_RELAYSESSION>* relaySessions,
	const ServiceType& type, Server * server, _INT32 iface, Client * client)
{
	_INT32 _outgoing_iface = -1;

	if (relaySessions->find(client->Get_ID()) == relaySessions->end())
	{
		_outgoing_iface = DHCP_Functions::Handle_Relayed_Packet(type, server, iface, client->response);
		client->Set_Client_Hint(INADDR_BROADCAST, 68);
		client->response->set_flags(DHCP_FLAGS::Broadcast);
	}
	else
	{
		client->response->set_relayIP(relaySessions->at(client->Get_ID()).Get_RelayIP());
		client->Set_Client_Hint(relaySessions->at(client->Get_ID()).Get_RemoteIP(), 67);
		_outgoing_iface = relaySessions->at(client->Get_ID()).Get_Interface();
		
		client->response->set_flags(DHCP_FLAGS::Unicast);
		
		relaySessions->erase(client->Get_ID());
	}

	if (_outgoing_iface == -1)
		return;

	switch (type)
	{
	case DHCP_SERVER:


		switch (client->Get_DHCP_Client()->Get_Vendor())
		{
		case PXEClient:
			/*
				Before we respond to the client we should fixup the server in the Packet, So that clients
				talks to us instead to the upstream server. We leave all other options as they are and adding the
				responding server to the list of boot servers later, so that the client can also boot from the upstream server.
			*/

			client->response->set_nextIP(server->Get_Interface(type, _outgoing_iface)->Get_IPAddress());
			client->response->set_servername(server->Get_Interface(type, _outgoing_iface)->Get_ServerName());

			client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(54),
				static_cast<_ULONG>(server->Get_Interface(type, _outgoing_iface)->Get_IPAddress())));
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	client->response->Commit();

	if (server->Get_Interface(type, _outgoing_iface)->Get_ServiceType() == type)
		server->Send(type, _outgoing_iface, client);
}

void DHCP_Functions::Add_BootServer_To_ServerList(std::vector<BootServerEntry>* serverlist,
	Server* server, Client * client, const std::string& serverName, const std::string& bootfile)
{
	std::vector<_IPADDR> addrs;
	addrs.emplace_back(client->Get_Server_Hint().sin_addr.s_addr);

	std::string _name = serverName;

	if (_name.size() == 0)
		_name = Functions::AddressStr(client->Get_Server_Hint().sin_addr.s_addr);

	DHCP_Functions::Add_BootServer(serverlist, std::string("(*) ") + _name, addrs, bootfile);
}

bool DHCP_Functions::Has_BootServer(std::vector<BootServerEntry>* serverlist, const _USHORT& id)
{
	if (serverlist->size() == 0 || serverlist->size() < id)
		return false;

	BootServerEntry* entry = &serverlist->at(id - 1);

	return entry != nullptr;
}

void DHCP_Functions::Add_BootServer(std::vector<BootServerEntry>* serverlist,
	const std::string& name, const std::vector <_IPADDR>& addresses, const std::string& bootfile)
{
	_USHORT id = static_cast<_USHORT>(serverlist->size() + 1);

	if (addresses.size() == 0)
	{
		printf("[E] Passed Bootserver %s with no addresses!\n", name.c_str());
		return;
	}

	if (Has_BootServer(serverlist, id))
	{
		printf("[D] Bootserver already in the List...\n");
		return;
	}

	serverlist->emplace_back(BootServerEntry(id, name, addresses, bootfile.size() == 0 ? "" : bootfile));
}

void DHCP_Functions::Handle_IPXE_Options(Server * server, _INT32 iface, Client * client, Packet * response)
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

void DHCP_Functions::Handle_WDS_Options(const SETTINGS* settings, const ServiceType& type, Server* server, _INT32 iface, Client * client)
{
	// WDS Options -> Used by WDSNBP
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
	{
		wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_REFERRAL_SERVER), client->Get_DHCP_Client()->Get_WDSClient()->GetReferalServer());
	}

	wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_ACTION_DONE), static_cast<_BYTE>(client->Get_DHCP_Client()->Get_WDSClient()->GetActionDone()));

	if (client->Get_DHCP_Client()->Get_WDSClient()->GetNextAction() == APPROVAL && client->Get_DHCP_Client()->Get_WDSClient()->GetWDSMessage().size() != 0)
	{
		wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_MESSAGE), client->Get_DHCP_Client()->Get_WDSClient()->GetWDSMessage());
	}

	wdsOptions->emplace_back(static_cast<_BYTE>(WDSBP_OPT_END));
	client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(250), *wdsOptions));

	wdsOptions->clear();
	delete wdsOptions;

	wdsOptions = nullptr;
}
