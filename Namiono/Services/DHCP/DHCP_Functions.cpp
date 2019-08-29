#include <Namiono/Namiono.h>

DHCP_Functions::DHCP_Functions()
{
}

DHCP_Functions::~DHCP_Functions()
{
}

void DHCP_Functions::Generate_Bootmenu_From_ServerList(std::vector<BootServerEntry>* serverlist, Client* client)
{
	if (serverlist->size() == 0)
		return;

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

	for (_USHORT i = 0; i < serverlist->size(); i++)
	{
		if (serverlist->at(i).Description.size() == 0)
			continue;

		/* id */
		id = htons(serverlist->at(i).Ident);
		memcpy(&menubuffer[offset], &id, sizeof id);
		offset += sizeof id;

		/* desc len */
		length = static_cast<_BYTE>(strlen(serverlist->at(i).Description.c_str()));
		memcpy(&menubuffer[offset], &length, sizeof length);
		offset += sizeof length;

		/* desc */
		memcpy(&menubuffer[offset], serverlist->at(i).Description.c_str(), length);
		offset += length;
	}

	client->dhcp->vendorOpts->emplace_back(static_cast<_BYTE>(PXE_BOOT_MENU),
		static_cast<_BYTE>(offset), menubuffer);

	/* Menue prompt */
	_BYTE timeout = SETTINGS.PXEPROMPTTIMEOUT;
	const std::string prompt = timeout != static_cast<_BYTE>(0xff) ?
		SETTINGS.PXEPROMP : std::string("These servers (*) responded to your request...");

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

void DHCP_Functions::Create_BootServerList(std::vector<BootServerEntry>* serverlist, Client* client)
{
	_BYTE offset = 0;
	_BYTE ipcount = 0;
	char* serverbuffer = new char[1024];
	ClearBuffer(serverbuffer, 1024);

	if (serverlist->size() == 0)
		return;

	for (_USHORT i = 0; i < serverlist->size(); i++)
	{
		if (serverlist->at(i).Addresses.size() == 0 ||
			serverlist->at(i).Description.size() == 0)
			continue;

		_USHORT id = htons(serverlist->at(i).Ident);
		memcpy(&serverbuffer[offset], &id, sizeof id);
		offset += sizeof id;

		ipcount = static_cast<_BYTE>(serverlist->at(i).Addresses.size());

		memcpy(&serverbuffer[offset], &ipcount, sizeof ipcount);
		offset += sizeof ipcount;

		for (const _IPADDR& address : serverlist->at(i).Addresses)
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

void DHCP_Functions::Relay_Request_Packet(const _IPADDR& addr, const _USHORT& port,
	const ServiceType& type, Server * server, int iface, Client * client)
{
	/* Client request */
	client->dhcp->Set_State(CLIENTSTATE::DHCP_RELAY);
	client->Set_Relay_Hint(addr, port);
	client->response->set_relayIP(server->Get_Interface(iface)->Get_IPAddress());
	DHCP_Functions::Handle_Relayed_Packet(server, iface, client->response);

	std::string mac = Functions::MacAsString(&client->response->Get_Buffer()[28],
		static_cast<_SIZET>(client->response->get_hwlength())).c_str();

	/*
		Store the mac in our cache on the incoming interface, so that we can later
		identify the Interface from where we got the request.
		
		The Interface id is already stored in option 82-> [1] but it must not be the right one.
	*/

	if (!server->Get_Interface(iface)->Has_ARPEntry(mac))
	{
		server->Get_Interface(iface)->Add_ARPEntry(mac);
	}

	/*
	Put the IP Address from the interface which received the request in the giaddr field...
	The Upstream DHCP server will send this response back to this address in the giaddr field...

	We need also the giaddr to identify the interface... when receiving responses...
	*/

	switch (client->dhcp->Get_Vendor())
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
		server->Send(iface.Get_Id(), client);
}
_INT32 DHCP_Functions::Handle_Relayed_Packet(Server * server, int iface, Packet * packet)
{
	_INT32 _interface = iface;
	std::string remoteid;

	if (packet->Has_DHCPOption(static_cast<_BYTE>(82)))
	{
		printf("[D] Got Relay Agent Options:\n");

		std::vector<DHCP_Option> options;
		packet->Get_DHCPOption(82).Get_SubOptions(options);

		for (_SIZET i = 0; i < options.size(); i++)
		{
			if (options.at(i).Option == CircuitID)
			{
				_interface = options.at(i).Get_Value_As_Byte();
			}

			if (options.at(i).Option == RemoteID)
			{
				remoteid = options.at(i).Get_Value_As_String();
			}
		}

		packet->Remove_DHCPOption(static_cast<_BYTE>(82));
	}
	else
	{
		std::vector<DHCP_Option> relayOptions;
		relayOptions.emplace_back(DHCP_Option(static_cast<_BYTE>(RemoteID), server->Get_Interface(iface)->Get_ServerName()));
		relayOptions.emplace_back(DHCP_Option(static_cast<_BYTE>(CircuitID), static_cast<_BYTE>(iface)));

		packet->Add_DHCPOption(DHCP_Option(82, relayOptions));
	}

	return _interface;
}

void DHCP_Functions::Relay_Response_Packet(const ServiceType& type, Server * server, int iface, Client * client)
{
	/* Server response */
	client->Set_Client_Hint(INADDR_BROADCAST, 68);
	client->response->set_flags(DHCP_FLAGS::Broadcast);
	
	std::string mac = Functions::MacAsString(&client->response->Get_Buffer()[28],
		static_cast<_SIZET>(client->response->get_hwlength())).c_str();

	int _outgoing_iface = server->Get_Interface_by_Address(client->response->get_relayIP());
	
	/* Send it out to the receiving Interface ... anyway ... */
	if (_outgoing_iface == -1)
	{
		printf("[W] Can not find Interface with address %s...\n",
			Functions::AddressStr(client->response->get_relayIP()).c_str());

		_outgoing_iface = DHCP_Functions::Handle_Relayed_Packet(server, iface, client->response);
	}

	switch (client->dhcp->Get_Vendor())
	{
	case PXEClient:

		/*
			Before we respond to the client we should fixup the server in the Packet, So that clients
			talks to us instead to the upstream server. We leave all other options as they are and adding the
			responding server to the list of boot servers later, so that the client can boot from the upstream server.
		*/

		client->response->set_nextIP(server->Get_Interface(_outgoing_iface)->Get_IPAddress());
		client->response->set_servername(server->Get_Interface(_outgoing_iface)->Get_ServerName());

		client->response->Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(54),
			static_cast<_ULONG>(server->Get_Interface(iface)->Get_IPAddress())));
		break;
	default:
		break;
	}

	client->dhcp->Set_State(CLIENTSTATE::DHCP_DONE);
	client->response->Commit();
	server->Send(_outgoing_iface, client);
}

void DHCP_Functions::Add_BootServer_To_ServerList(std::vector<BootServerEntry>* serverlist,
	Server* server, Client * client, const std::string& serverName)
{
	std::vector<_IPADDR> addrs;
	addrs.emplace_back(client->Get_Client_Hint().sin_addr.s_addr);

	std::string _name = serverName;

	if (_name.size() == 0)
		_name = Functions::AddressStr(client->Get_Client_Hint().sin_addr.s_addr);

	DHCP_Functions::Add_BootServer(serverlist, _name, addrs);
}

bool DHCP_Functions::Has_BootServer(std::vector<BootServerEntry>* serverlist, const _USHORT& id)
{
	if (serverlist->size() == 0 || serverlist->size() < id)
		return false;

	BootServerEntry* entry = &serverlist->at(id - 1);

	return entry != nullptr;
}

void DHCP_Functions::Add_BootServer(std::vector<BootServerEntry>* serverlist, const std::string& name, const std::vector <_IPADDR>& addresses)
{
	_USHORT id = static_cast<_USHORT>(serverlist->size() + 1);

	if (addresses.size() == 0)
	{
		printf("[E] Passed Bootserver %s with no addresses!\n", name.c_str());
		return;
	}

	if (Has_BootServer(serverlist, id))
		return;

	serverlist->emplace_back(BootServerEntry(id, name, addresses, ""));
}

void DHCP_Functions::Handle_IPXE_Options(Server * server, int iface, Client * client, Packet * response)
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

void DHCP_Functions::Handle_WDS_Options(Server * server, int iface, Client * client)
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
