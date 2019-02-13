#include <Namiono-Service-DHCP.h>


void Namiono_Service_DHCP::Update()
{
}

Namiono_Service_DHCP::Namiono_Service_DHCP(Server* server)
{
	sqlite3_open_v2("Computers.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
}

Namiono_Service_DHCP::~Namiono_Service_DHCP()
{
	Serverlist.clear();
	sqlite3_close_v2(db);
}

void Namiono_Service_DHCP::Create_BootServer_List(Server* server, const std::string& ident, std::vector<DHCP_Option>& vendorOps)
{
	Serverlist.clear();

	addresses.emplace_back(server->Get_IPAddress(ident));
	Serverlist.emplace_back(id, server->Get_Hostname(), addresses, "");
	addresses.clear();

	int ret = sqlite3_prepare_v2(db, "SELECT Description, Address, Type, File FROM Servers", -1, &stmt, NULL);

	if (ret != SQLITE_OK)
		return;

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		std::vector<std::string> splits = Functions::Split(std::string((const char*)sqlite3_column_text(stmt, 1)), "|");

		for (std::string& entry : splits)
			addresses.emplace_back(inet_addr(entry.c_str()));

		Serverlist.emplace_back(++id, std::string((const char*)sqlite3_column_text(stmt, 0)),
			addresses, std::string((const char*)sqlite3_column_text(stmt, 3)));

		addresses.clear();
	}

	sqlite3_finalize(stmt);

	if (Serverlist.size() == 0)
		return;

	unsigned char offset = 0;
	char* serverbuffer = new char[1024];
	ClearBuffer(serverbuffer, sizeof serverbuffer);

	for (BootServerEntry& entry : Serverlist)
	{
		memcpy(&serverbuffer[offset], &entry.Ident, sizeof entry.Ident);
		offset += sizeof entry.Ident;

		unsigned char ipcount = static_cast<unsigned char>(entry.Addresses.size());

		memcpy(&serverbuffer[offset], &ipcount, sizeof ipcount);
		offset += sizeof ipcount;

		for (_IPADDR & address : entry.Addresses)
		{
			memcpy(&serverbuffer[offset], &address, sizeof address);
			offset += sizeof address;
		}
	}

	vendorOps.emplace_back(static_cast<unsigned char>(PXE_BOOT_SERVERS),
		static_cast<unsigned char>(offset), serverbuffer);
	
	delete[] serverbuffer;
}

void Namiono_Service_DHCP::Handle_DHCP_Discover(Server* server, const std::string& ident, Client* client, Packet* packet)
{
	Packet* response = new Packet(*packet, 1260, DHCP_MSGTYPE::OFFER);

	response->Add_DHCPOption(DHCP_Option(54, server->Get_IPAddress(ident)));
	response->set_servername(server->Get_Hostname());
	response->set_nextIP(server->Get_IPAddress(ident));

	std::vector<DHCP_Option> vendorOpts;
	if (SETTINGS.PXEBOOTMENUE == 1)
	{
		vendorOpts.emplace_back(static_cast<unsigned char>
			(PXE_DISCOVERY_CONTROL), static_cast<unsigned char>(SETTINGS.DISCOVERY_MODE));

		Create_BootServer_List(server, ident, vendorOpts);
		GenerateBootMenue(client, &vendorOpts);
	}

	vendorOpts.emplace_back(static_cast<unsigned char>(PXE_DISCOVERY_MCADDR),
		static_cast<_IPADDR>(inet_addr(SETTINGS.DISCOVERY_ADDR.c_str())));

	vendorOpts.emplace_back(static_cast<unsigned char>(PXE_MTFTP_IP_ADDR),
		static_cast<_IPADDR>(inet_addr(SETTINGS.DISCOVERY_ADDR.c_str())));

	vendorOpts.emplace_back(static_cast<unsigned char>(PXE_MTFTP_SERVER_PORT), SETTINGS.MTFTP_SPORT);
	vendorOpts.emplace_back(static_cast<unsigned char>(PXE_MTFTP_CLIENT_PORT), SETTINGS.MTFTP_CPORT);

	if (vendorOpts.size() != 0)
		response->Add_DHCPOption(DHCP_Option(static_cast<unsigned char>(43), vendorOpts));

	response->Commit();

	int retval = server->Send(ident, client, response);
	if (retval == SOCKET_ERROR)
		printf("Socket Error while sending packet!\n");

	delete response;

	client->DHCP->Set_State(DHCP_DONE);
}

void Namiono_Service_DHCP::GenerateBootMenue(const Client* client, std::vector<DHCP_Option>* vendorOpts)
{
	_SIZE_T offset = 0;
	char menubuffer[1024];

	ClearBuffer(menubuffer, sizeof menubuffer);

	unsigned short id = 0;
	memcpy(&menubuffer[offset], &id, sizeof id);
	offset += sizeof id;

	/* desc len */
	unsigned char length = static_cast<unsigned char>(strlen("Local Boot"));
	memcpy(&menubuffer[offset], &length, sizeof length);
	offset += sizeof length;

	/* desc */
	memcpy(&menubuffer[offset], "Local Boot", length);
	offset += length;

	for (BootServerEntry & entry : Serverlist)
	{
		id = entry.Ident;

		memcpy(&menubuffer[offset], &id, sizeof id);
		offset += sizeof id;

		/* desc len */
		length = static_cast<unsigned char>(strlen(entry.Description.c_str()));
		memcpy(&menubuffer[offset], &length, sizeof length);
		offset += sizeof length;

		/* desc */
		memcpy(&menubuffer[offset], entry.Description.c_str(), length);
		offset += length;
	}

	vendorOpts->emplace_back(static_cast<unsigned char>(PXE_BOOT_MENU),
		static_cast<unsigned char>(offset), menubuffer);

	/* Menue prompt */
	const std::string prompt = SETTINGS.PXEPROMP;

	char* promptbuffer = new char[prompt.size() + 1];
	ClearBuffer(menubuffer, prompt.size());
	offset = 0;

	unsigned char timeout = SETTINGS.PXEPROMPTTIMEOUT;

	memcpy(&promptbuffer[offset], &timeout, sizeof(unsigned char));
	offset += sizeof(unsigned char);

	memcpy(&promptbuffer[offset], prompt.c_str(),
		static_cast<_SIZE_T>(prompt.size()));

	offset += static_cast<_SIZE_T>(prompt.size());

	vendorOpts->emplace_back(static_cast<unsigned char>(PXE_MENU_PROMPT),
		static_cast<unsigned char>(offset), promptbuffer);

	delete[] promptbuffer;
}

void Namiono_Service_DHCP::Handle_WDS_Options(Server* server, const std::string& ident, Client* client, Packet* response)
{

	if (client->DHCP->GetIsWDSRequest())
	{
		this->requestId++;
		response->set_filename(client->DHCP->GetBootfile());
		client->DHCP->wds->SetRequestID(this->requestId);
	}

	if (client->DHCP->wds->GetBCDfile().size() != 0)
		response->Add_DHCPOption(DHCP_Option(252, client->DHCP->wds->GetBCDfile()));

	client->DHCP->Set_State(client->DHCP->wds->
		GetActionDone() == 0 ? DHCP_WAITING : DHCP_DONE);

	// WDS Option -> Used by WDSNBP
	std::vector<DHCP_Option>* wdsOptions = new std::vector<DHCP_Option>();
	wdsOptions->emplace_back(WDSBP_OPT_NEXT_ACTION, static_cast<unsigned char>(client->DHCP->wds->GetNextAction()));
	wdsOptions->emplace_back(WDSBP_OPT_REQUEST_ID, static_cast<unsigned long>(BS32(client->DHCP->wds->GetRequestID())));

	wdsOptions->emplace_back(WDSBP_OPT_POLL_INTERVAL, static_cast<unsigned short>(BS16(client->DHCP->wds->GetPollInterval())));
	wdsOptions->emplace_back(WDSBP_OPT_POLL_RETRY_COUNT, static_cast<unsigned short>(BS16(client->DHCP->wds->GetRetryCount())));

	client->DHCP->wds->SetReferralServer(server->Get_IPAddress(ident));
	if (client->DHCP->wds->GetNextAction() == REFERRAL)
	{
		wdsOptions->emplace_back(WDSBP_OPT_REFERRAL_SERVER, client->DHCP->wds->GetReferalServer());
		wdsOptions->emplace_back(WDSBP_OPT_ALLOW_SERVER_SELECTION, static_cast<unsigned char>
			(this->Serverlist.size() != 0 ? 1 : 0));
	}

	wdsOptions->emplace_back(WDSBP_OPT_ACTION_DONE, static_cast<unsigned char>(client->DHCP->wds->GetActionDone()));

	if (client->DHCP->wds->GetWDSMessage().size() != 0)
		wdsOptions->emplace_back(WDSBP_OPT_MESSAGE, client->DHCP->wds->GetWDSMessage());

	response->Add_DHCPOption(DHCP_Option(250, *wdsOptions));
	wdsOptions->clear();
	delete wdsOptions;

}

void Namiono_Service_DHCP::Handle_DHCP_Request(Server* server, const std::string& ident, Client* client, Packet* packet)
{
	Packet* response = new Packet(*packet, 1024, DHCP_MSGTYPE::ACK);
	response->Add_DHCPOption(DHCP_Option(54, server->Get_IPAddress(ident)));
	response->set_servername(server->Get_Hostname());
	response->set_nextIP(server->Get_IPAddress(ident));

	std::vector<DHCP_Option> vendorOpts;
	switch (client->DHCP->Get_Vendor())
	{
	case PXEServer:
	case PXEClient:
		if (packet->Has_DHCPOption(43))
		{
			char vendorbuffer[512];
			ClearBuffer(vendorbuffer, sizeof vendorbuffer);

			memcpy(vendorbuffer, packet->Get_DHCPOption(43).Value, packet->Get_DHCPOption(43).Length);

			std::vector<DHCP_Option> options;

			for (_SIZE_T i = 0; i < packet->get_Length(); i++)
			{
				if (static_cast<unsigned char>(vendorbuffer[i]) == static_cast<unsigned char>(0xff) ||
					static_cast<unsigned char>(vendorbuffer[i]) == 0x00)
					break;

				options.emplace_back(static_cast<unsigned char>(vendorbuffer[i]),
					static_cast<unsigned char>(vendorbuffer[i + 1]), &vendorbuffer[i + 2]);

				i += 1 + vendorbuffer[i + 1];
			}

			// Get the selected Server...
			for (DHCP_Option & opt : options)
				if (opt.Option == static_cast<unsigned char>(PXE_BOOTITEM))
				{
					unsigned short layer = 0;
					unsigned short type = 0;

					memcpy(&type, &opt.Value[0], sizeof(unsigned short));
					if (type != 0)
						client->DHCP->rbcp->Set_Item(type);

					memcpy(&layer, &opt.Value[2], sizeof(unsigned short));
					if (layer != 0)
						client->DHCP->rbcp->Set_Layer(layer);
				}
		}
		
		switch (client->DHCP->rbcp->Get_Item())
		{
		case PXEAPITestServer:
			client->DHCP->SetBootfile("Boot/x86/apitest.0");
			break;
		case WindowsNTBootServer:
		default:
			if (packet->Has_DHCPOption(250))
			{
				char* vendorbuffer = new char[packet->Get_DHCPOption(250).Length];
				ClearBuffer(vendorbuffer, packet->Get_DHCPOption(250).Length);

				memcpy(vendorbuffer, packet->Get_DHCPOption(250).Value, packet->Get_DHCPOption(250).Length);

				std::vector<DHCP_Option> options;

				for (_SIZE_T i = 0; i < packet->get_Length(); i++)
				{
					if (static_cast<unsigned char>(vendorbuffer[i]) == static_cast<unsigned char>(0xff) ||
						static_cast<unsigned char>(vendorbuffer[i]) == 0x00)
						break;

					options.emplace_back(static_cast<unsigned char>(vendorbuffer[i]),
						static_cast<unsigned char>(vendorbuffer[i + 1]), &vendorbuffer[i + 2]);

					i += 1 + vendorbuffer[i + 1];
				}

				delete[] vendorbuffer;

				for (DHCP_Option & opt : options)
					if (opt.Option == static_cast<unsigned char>(WDSBP_OPT_ARCHITECTURE))
					{
						DHCP_ARCH arch = INTEL_X86;

						memcpy(&arch, &opt.Value[0], sizeof(unsigned short));
						client->DHCP->SetArchitecture(arch);
						response->set_filename(client->DHCP->GetBootfile());

					}

				Handle_WDS_Options(server, ident, client, response);
			}
			else
				response->set_filename("Boot/x86/wdsnbp.com");
			break;
		}



		if (client->DHCP->rbcp->Get_Item() != 0)
		{
			char item[4];
			unsigned short type = client->DHCP->rbcp->Get_Item();
			unsigned short layer = client->DHCP->rbcp->Get_Layer();

			memcpy(&item[0], &type, sizeof type);
			memcpy(&item[2], &layer, sizeof layer);

			vendorOpts.emplace_back(static_cast<unsigned char>(PXE_BOOTITEM), static_cast<unsigned char>(4), item);
		}

		if (vendorOpts.size() != 0)
			response->Add_DHCPOption(DHCP_Option(static_cast<unsigned char>(43), vendorOpts));
		break;
	}
	
	response->Commit();

	int retval = SOCKET_ERROR;
	if (client->DHCP->GetIsWDSRequest())
		if (client->DHCP->wds->GetActionDone() == 0)
			return;

	retval = server->Send(ident, client, response);

	if (retval == SOCKET_ERROR)
		printf("Socket Error while sending packet!\n");

	delete response;
}

void Namiono_Service_DHCP::Handle_Service_Request(Server* server, const std::string& ident, Client* client, Packet* packet)
{
	if (client == nullptr)
		return;

	// Drop Packet when the Hop count is to hight...
	if (packet->get_hops() > SETTINGS.MAX_HOPS)
	{
		printf("[W] %s: Got PXE packet with more than %d hops... -> DROPPING!\n", ident.c_str(),SETTINGS.MAX_HOPS);
		return;
	}

	if (Functions::Compare(packet->Get_DHCPOption(60).Value, "PXEClient", 9))
		client->DHCP->Set_Vendor(PXEClient);
	else
	{
		if (Functions::Compare(packet->Get_DHCPOption(60).Value, "PXEServer", 9))
			client->DHCP->Set_Vendor(PXEServer);
		else
		{
			if (Functions::Compare(packet->Get_DHCPOption(60).Value, "APPLEBSDP", 9))
				client->DHCP->Set_Vendor(APPLEBSDP);
			else
				return;
		}
	}

	if (packet->Has_DHCPOption(54))
	{
		char myIP[4];

		memcpy(myIP, &server->Get_IPAddress(ident), 4);

		if (!Functions::Compare(packet->Get_DHCPOption(54).Value, myIP, sizeof myIP))
			client->DHCP->Set_State(DHCP_ABORT);
	}

	if (client->DHCP->Get_State() != DHCP_ABORT)
	{
		client->DHCP->SetIsWDSRequest(packet->Has_DHCPOption(250));
		DHCP_MSGTYPE msgType = static_cast<DHCP_MSGTYPE>(packet->Get_DHCPOption(53).Value[0]);
		switch (msgType)
		{
		case DHCP_MSGTYPE::DISCOVER:
			this->Handle_DHCP_Discover(server, ident, client, packet);
			break;
		case DHCP_MSGTYPE::INFORM: // INFORMs are mostly the same as request
		case DHCP_MSGTYPE::REQUEST:
			this->Handle_DHCP_Request(server, ident, client, packet);
			break;
		}
	}

	if (client->DHCP->Get_State() == DHCP_DONE || client->DHCP->Get_State() == DHCP_ABORT)
		server->Remove_Client(client->Get_Ident());
}
