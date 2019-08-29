#include <Namiono/Namiono.h>
using namespace Namiono::Network;

namespace Namiono
{
	namespace Services
	{
		TFTP_Service::TFTP_Service(const std::string& rootDir)
		{
			this->_rootDir = rootDir;
		}

		void TFTP_Service::Handle_Service_Request(const ServiceType& type, Namiono::Network::Server* server, int iface,
			Namiono::Network::Client* client, Namiono::Network::Packet* packet)
		{
			switch (static_cast<TFTP_OPCODE>(packet->Get_Opcode()))
			{
			case Packet_OPCode::TFTP_RRQ:
				Handle_RRQ_Request(type, server, iface, client, packet);
				break;
			case Packet_OPCode::TFTP_ACK:
				Handle_ACK_Request(type, server, iface, client, packet);
				break;
			case Packet_OPCode::TFTP_ERR:
				Handle_ERR_Request(type, server, iface, client, packet);
				break;
			}

			if (client->tftp->Get_State() == CLIENTSTATE::TFTP_DONE
				|| client->tftp->Get_State() == CLIENTSTATE::TFTP_ERROR)
				server->Remove_Client(client->Get_ID());
		}

		void TFTP_Service::Handle_RRQ_Request(const ServiceType& type, Namiono::Network::Server* server, int iface,
			Namiono::Network::Client* client, Namiono::Network::Packet* packet)
		{
			client->tftp = new TFTP_CLIENT();
			client->tftp->SetFilename(Combine(this->_rootDir, std::string(packet->Get_TFTPOption("File").Value)));

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

				server->Send(iface, client);

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
				server->Send(iface, client);

				delete client->response;
				client->response = nullptr;

				client->tftp->Set_State(TFTP_DOWNLOAD);
				return;
			}

			client->tftp->Set_State(TFTP_ERROR);
		}

		void TFTP_Service::Handle_ACK_Request(const ServiceType& type, Namiono::Network::Server* server, int iface,
			Namiono::Network::Client* client, Namiono::Network::Packet* packet)
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

		void TFTP_Service::Handle_ERR_Request(const ServiceType& type, Namiono::Network::Server* server, int iface,
			Namiono::Network::Client* client, Namiono::Network::Packet* packet)
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


		TFTP_Service::~TFTP_Service()
		{
		}
	}
}
