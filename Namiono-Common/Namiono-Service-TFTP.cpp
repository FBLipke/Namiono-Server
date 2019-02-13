#include <Namiono-Service-TFTP.h>

Namiono_Service_TFTP::Namiono_Service_TFTP()
{
}


Namiono_Service_TFTP::~Namiono_Service_TFTP()
{
}

void Namiono_Service_TFTP::Update()
{
}

void Namiono_Service_TFTP::Handle_Service_Request(Server* server, const std::string & ident, Client * client, Packet * packet)
{
	if (client == nullptr)
		return;

	switch (packet->Get_Opcode())
	{
	case TFTP_RRQ:
		this->Handle_TFTP_RRQ(server, ident, client, packet);
		break;
	case TFTP_ACK:
		this->Handle_TFTP_ACK(server, ident, client, packet);
		break;
	default:
		client->TFTP->SetTFTPState(TFTP_ERROR);
		break;
	}

	if (client->TFTP->GetTFTPState() == TFTP_DONE || client->TFTP->GetTFTPState() == TFTP_ERROR)
		server->Remove_Client(client->Get_Ident());
}

_SIZE_T ReadFile(char* dstBuffer, const std::string& file, const _SIZE_T index, const _SIZE_T count)
{
	_SIZE_T bytesRead = 0;

	FILE* fil = fopen(file.c_str(), "rb");

	if (!fil)
	{
		printf("[E] %s: Error while reading file: %s\n", __FUNCTION__, file.c_str());
		return -1;
	}

	fseek(fil, index, SEEK_SET);


	bytesRead = static_cast<_SIZE_T>(fread(&dstBuffer, 1, count, fil));

	fclose(fil);

	return bytesRead;
}

_SIZE_T FileLength(const std::string& file)
{
	_SIZE_T bytes = 0;

	FILE* fil = fopen(file.c_str(), "rb");

	if (!fil)
		return -1;

	fseek(fil, 0, SEEK_END);

	bytes = ftell(fil);
	rewind(fil);
	fclose(fil);

	return bytes;
}

void Namiono_Service_TFTP::Handle_TFTP_ERR(Server* server, const std::string& ident, Client* client, Packet* packet)
{
}

void Namiono_Service_TFTP::Handle_TFTP_RRQ(Server* server, const std::string& ident, Client* client, Packet* packet)
{
	client->TFTP.get()->SetFilename(Functions::Combine(std::string("[TFTP]"),
		Functions::Combine(client->DHCP->GetPrefix(),std::string(packet->Get_TFTPOption("File").Value))));
	
	client->TFTP.get()->SetBytesToRead(FileLength(client->TFTP.get()->GetFilename()));

	if (!Functions::FileExist(client->TFTP.get()->GetFilename().c_str()))
	{
		client->TFTP->SetTFTPState(TFTP_ERROR);
		std::unique_ptr<Packet> response(new Packet((client->TFTP.get()->GetFilename().size() + 1) + 4, Packet_OPCode::TFTP_ERR));
		response->Write(static_cast<unsigned short>(BS16(5)));
		response->Write(static_cast<unsigned short>(BS16(1)), 2);
		
		response->Write(client->TFTP.get()->GetFilename().c_str(),
			client->TFTP.get()->GetFilename().size(), 4);
		response->Commit();

		server->Send(ident, client, response.get());
		return;
	}

	std::unique_ptr<Packet> response(new Packet(1024, Packet_OPCode::TFTP_OACK));

	if (packet->Has_TFTPOption("tsize"))
		response->Add_TFTPOption(TFTP_Option("tsize", Functions::AsString(client->TFTP->GetBytesToRead())));

	if (packet->Has_TFTPOption("blksize"))
	{
		char blksize[6];
		memcpy(&blksize, packet->Get_TFTPOption("blksize").Value, sizeof blksize);
		client->TFTP->SetBlockSize(atoi(blksize));
	}

	response->Add_TFTPOption(TFTP_Option("blksize", Functions::AsString(client->TFTP->GetBlockSize())));

	if (packet->Has_TFTPOption("windowsize"))
	{
		char winsize[6];
		memcpy(&winsize, packet->Get_TFTPOption("windowsize").Value, sizeof winsize);
		client->TFTP->SetWindowSize(atoi(winsize));
		response->Add_TFTPOption(TFTP_Option("windowsize", Functions::AsString(client->TFTP->GetWindowSize())));
	}

	if (packet->Has_TFTPOption("msftwindow"))
	{
		char winsize[6];
		memcpy(&winsize, packet->Get_TFTPOption("msftwindow").Value, sizeof winsize);
		client->TFTP->SetMSFTWindow(atoi(winsize));
		response->Add_TFTPOption(TFTP_Option("msftwindow", Functions::AsString(client->TFTP->GetMSFTWindow())));
	}

	response->Commit();
	server->Send(ident, client, response.get());

	client->TFTP->SetTFTPState(TFTP_DOWNLOAD);
}

void Namiono_Service_TFTP::Handle_TFTP_ACK(Server* server, const std::string& ident, Client* client, Packet* packet)
{
	if (client->TFTP == nullptr || client == nullptr)
		return;

	if (client->TFTP->GetTFTPState() != TFTP_DOWNLOAD)
	{
		client->TFTP->SetTFTPState(TFTP_ERROR);

		return;
	}

	if (client->TFTP.get()->GetCurrentBlock() != packet->Get_Block())
	{
		client->TFTP->SetTFTPState(TFTP_ERROR);
		return;
	}

	if (packet->get_Length() > 4)
	{
		// Clients can send new window size via ACK at pos 4!
		client->TFTP->SetWindowSize(static_cast<unsigned short>(packet->Get_Buffer()[4]));
	}

	FILE* fil = fopen(client->TFTP.get()->GetFilename().c_str(), "rb");
	_SIZE_T chunk = 0;
	
	for (unsigned short i = 0; i < client->TFTP->GetWindowSize(); i++)
	{
		chunk = ((_SIZE_T)client->TFTP.get()->GetBlockSize() < (client->TFTP.get()->GetBytesToRead() - client->TFTP.get()->GetBytesRead())) ?
			(_SIZE_T)client->TFTP.get()->GetBlockSize() : (client->TFTP.get()->GetBytesToRead() - client->TFTP.get()->GetBytesRead());

		if (fseek(fil, client->TFTP.get()->GetBytesRead(), SEEK_SET) != 0)
			break;

		client->TFTP.get()->SetCurrentBlock(client->TFTP.get()->GetCurrentBlock() + 1);
		std::unique_ptr<Packet> response(new Packet(4 + chunk, Packet_OPCode::TFTP_DAT));
		response.get()->Set_Block(BS16(client->TFTP.get()->GetCurrentBlock()));

		client->TFTP.get()->SetBytesRead(static_cast<long>(fread(&response.get()->Get_Buffer()[4], 1, chunk, fil)));

		response.get()->Commit();
		server->Send(ident, client, response.get());


		if (client->TFTP.get()->GetBytesRead() == client->TFTP.get()->GetBytesToRead())
		{
			client->TFTP->SetTFTPState(TFTP_DONE);
			break;
		}
	}

	fclose(fil);
}