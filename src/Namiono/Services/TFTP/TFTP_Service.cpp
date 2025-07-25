#include <Namiono/Namiono.h>
using namespace Namiono::Network;

namespace Namiono
{
	namespace Services
	{
		TFTP_Service::TFTP_Service(SETTINGS* settings, const std::string& rootDir)
		{
			this->_rootDir = rootDir;
			this->settings = settings;
		}

		void TFTP_Service::Handle_Service_Request(const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
			Namiono::Network::Client* client, Namiono::Network::Packet* packet)
		{
			switch (static_cast<Packet_OPCode>(packet->Get_Opcode()))
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

			if (client->Get_TFTP_Client()->Get_State() == CLIENTSTATE::TFTP_DONE
				|| client->Get_TFTP_Client()->Get_State() == CLIENTSTATE::TFTP_ERROR)
				server->Remove_Client(client->Get_ID());
		}

		void TFTP_Service::Handle_RRQ_Request(const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
			Namiono::Network::Client* client, Namiono::Network::Packet* packet)
		{
			if (packet->Get_TFTPOption("File").Length == 0)
			{
				client->Get_TFTP_Client()->Set_State(TFTP_ERROR);
				return;
			}

			client->Get_TFTP_Client()->SetFilename(Combine(this->_rootDir, std::string(packet->Get_TFTPOption("File").Value)));
			printf("[I] TFTP : File requested: %s\n", client->Get_TFTP_Client()->GetFilename().c_str());

			if (!FileExist(client->Get_TFTP_Client()->GetFilename()))
			{
				printf("[E] TFTP : File not found: %s\n", client->Get_TFTP_Client()->GetFilename().c_str());

				client->SetResponse(type, static_cast<_SIZET>((client->Get_TFTP_Client()->
					GetFilename().size() + 1) + 4), Packet_OPCode::TFTP_ERR);

				client->GetResponse()->Write(static_cast<_USHORT>(htons(5)), 0);
				client->GetResponse()->Write(static_cast<_USHORT>(htons(1)), 2);

				client->GetResponse()->Write(client->Get_TFTP_Client()->GetFilename().c_str(),
					client->Get_TFTP_Client()->GetFilename().size(), 4);

				client->GetResponse()->Commit();

				server->Send(type, iface, client);

				client->Get_TFTP_Client()->Set_State(TFTP_ERROR);
				return;
			}

			if (client->Get_TFTP_Client()->OpenFile(client->Get_TFTP_Client()->GetFilename()))
			{
				client->SetResponse(type, 1024, Packet_OPCode::TFTP_OACK);

				if (packet->Has_TFTPOption("tsize"))
					client->GetResponse()->Add_TFTPOption(TFTP_Option("tsize", Functions::AsString(client->Get_TFTP_Client()->GetBytesToRead())));

				if (packet->Has_TFTPOption("blksize"))
				{
					client->Get_TFTP_Client()->SetBlockSize(Functions::AsUSHORT(packet->Get_TFTPOption("blksize").Value));
				}

				client->GetResponse()->Add_TFTPOption(TFTP_Option("blksize", Functions::AsString(client->Get_TFTP_Client()->GetBlockSize())));

				client->GetResponse()->Commit();
				server->Send(type, iface, client);

				client->Get_TFTP_Client()->Set_State(TFTP_DOWNLOAD);
				return;
			}
		}

		void TFTP_Service::Handle_ACK_Request(const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
			Namiono::Network::Client* client, Namiono::Network::Packet* packet) {
			if (client->Get_TFTP_Client()->Get_State() != TFTP_DOWNLOAD)
			{
				client->Get_TFTP_Client()->Set_State(TFTP_ERROR);
				return;
			}

			_USHORT blk = client->Get_TFTP_Client()->GetCurrentBlock();
			_USHORT tmpblk = 0;
			_USHORT haveblk = 0;

			memcpy(&tmpblk, &packet->Get_Buffer()[2], sizeof(_USHORT));
			
			haveblk = htons(tmpblk);

			bool isInSync = memcmp(&haveblk, &blk, sizeof(blk)) == 0;

			if (!isInSync)
				client->Get_TFTP_Client()->ResetState(haveblk);

			if (packet->get_Length() > 4)
			{
				_BYTE newWinSize = 1;

				packet->Read(&newWinSize, sizeof newWinSize, 4);
				client->Get_TFTP_Client()->SetWindowSize(newWinSize);
				printf("[D] TFTP : Client requested new WindowSize: %d\n", newWinSize);
			}

			_SIZET chunk = 0;

			for (_BYTE i = 0; i < client->Get_TFTP_Client()->GetWindowSize(); i++)
			{
				chunk = static_cast<_SIZET>(client->Get_TFTP_Client()->GetBlockSize()) <
						client->Get_TFTP_Client()->GetBytesToRead() - client->Get_TFTP_Client()->GetBytesRead() ?
					static_cast<_SIZET>(client->Get_TFTP_Client()->GetBlockSize()) : (client->Get_TFTP_Client()->GetBytesToRead()
						- client->Get_TFTP_Client()->GetBytesRead());

				client->Get_TFTP_Client()->FileSeek();
				client->Get_TFTP_Client()->SetCurrentBlock(client->Get_TFTP_Client()->GetCurrentBlock() + 1);

				client->SetResponse(type, static_cast<_SIZET>(4 + chunk), Packet_OPCode::TFTP_DAT);
				client->GetResponse()->Set_Block(client->Get_TFTP_Client()->GetCurrentBlock());

				client->Get_TFTP_Client()->SetBytesRead(static_cast<_LONG>(fread(&client->GetResponse()->Get_Buffer()[4],
					1, chunk, client->Get_TFTP_Client()->Get_FileHandle())));

				client->GetResponse()->Commit();

				client->Get_TFTP_Client()->AddToBacklog(client->Get_TFTP_Client()->GetCurrentBlock(),
				client->Get_TFTP_Client()->GetBytesRead(), client->Get_TFTP_Client()->GetBytesToRead());
				
				server->Send(type, iface, client);

				if (client->Get_TFTP_Client()->GetBytesRead() == client->Get_TFTP_Client()->GetBytesToRead())
				{
					client->Get_TFTP_Client()->Set_State(TFTP_DONE);
					printf("[I] TFTP : (%s) Transfer completed!\n", client->Get_ID().c_str());
					client->Get_TFTP_Client()->CloseFile();
					break;
				}
			}
		}

		void TFTP_Service::Handle_ERR_Request(const ServiceType& type, Namiono::Network::Server* server, _USHORT iface,
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

			client->Get_TFTP_Client()->CloseFile();

			if (errcode != 0)
				client->Get_TFTP_Client()->Set_State(TFTP_ERROR);
		}

		TFTP_Service::~TFTP_Service()
		{
		}

		void TFTP_Service::Start()
		{
		}

		void TFTP_Service::Close()
		{
		}

		void TFTP_Service::Heartbeart()
		{
		}

		void TFTP_Service::Init()
		{
		}
	}
}
