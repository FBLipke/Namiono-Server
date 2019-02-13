#include "Namiono-Service-BINL.h"



Namiono_Service_BINL::Namiono_Service_BINL()
{
}


Namiono_Service_BINL::~Namiono_Service_BINL()
{
}

void Namiono_Service_BINL::Handle_BINL_NEG(Server* server, const std::string& ident, Client* client, Packet* packet)
{
	printf("NOT IMPLEMENTED! (SO FAR....)\n");
}

void Namiono_Service_BINL::Handle_BINL_RQU(Server* server, const std::string& ident, Client* client, Packet* packet)
{
	if (packet->Get_Buffer()[36] == 0xa)
	{
		std::string welcome = "<OSCML><META KEY=\"F3\" ACTION=\"REBOOT\"><META KEY=\"ENTER\" HREF=\"LOGIN\"> \
		<TITLE>Client Installation Wizard</TITLE><FOOTER>[F3] restart computer [ENTER] Continue</FOOTER> \
		<BODY left=5 right=75><BR>Welcome to the Client Installation Wizard,<BR>To Install a Operating \
		System Press [Enter]<BR><BR>To abort and restart the Computer press [F3].</BODY></OSCML>";

		Packet* response = new Packet(static_cast<_SIZE_T>(welcome.size() + 36), BINL_RSU);
		memcpy(&response->Get_Buffer()[8], &packet->Get_Buffer()[8], 28);
		memcpy(&response->Get_Buffer()[36], welcome.c_str(), welcome.size());
		response->Commit();

		server->Send(ident, client, response);

		delete response;
	}
	else
	{
		char oscfile[9];
		char tmp[4096];
		char _filename[255];

		ClearBuffer(oscfile, 9);
		ClearBuffer(tmp, 4096);
		ClearBuffer(_filename, sizeof _filename);

		strncpy(oscfile, &packet->Get_Buffer()[36], (packet->get_Length() - 28) - 1);
		sprintf(_filename, "english/%s.osc", oscfile);

		FILE* fil = fopen(_filename, "rb");
		if (fil == NULL)
			return;
		
		fseek(fil, 0, SEEK_END);
		_SIZE_T length = ftell(fil);
		rewind(fil);

		fread(tmp, 1, length, fil);
		fclose(fil);

		std::string oscContent = std::string(tmp);
		oscContent = Functions::Replace(oscContent, "%SERVERNAME%", server->Get_Hostname());
		oscContent = Functions::Replace(oscContent, "%SERVERDOMAIN%", "fblipke.de");
		oscContent = Functions::Replace(oscContent, "%MACHINENAME%", "Client01");

		Packet* response = new Packet(static_cast<_SIZE_T>(oscContent.size() + 36), BINL_RSU);
		memcpy(&response->Get_Buffer()[8], &packet->Get_Buffer()[8], 28);
		memcpy(&response->Get_Buffer()[36], oscContent.c_str(), oscContent.size());
		response->Commit();

		server->Send(ident, client, response);

		delete response;
	}
}

void Namiono_Service_BINL::Handle_Service_Request(Server * server, const std::string & ident, Client * client, Packet * packet)
{
	BINL_OPCODE msgtype;

	memcpy(&msgtype, &packet->Get_Buffer()[0], sizeof msgtype);
	printf("Packet Length: %d\n", packet->get_Length());
	switch (BS32(msgtype))
	{
	case BINL_RQU:
		printf("OSC File Request!\n");
		this->Handle_BINL_RQU(server, ident, client, packet);
		break;
	case BINL_NEG:
		printf("NTLMSSP NEGOTIATE Request!\n");
		this->Handle_BINL_NEG(server, ident, client, packet);
		break;
	default:
		break;
	}
}
