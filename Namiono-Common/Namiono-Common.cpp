#include <Namiono-Common.h>
namespace Namiono
{
	namespace Common
	{
		Server server;

		void __Update(const unsigned int interval = 60)
		{
			unsigned int sleep = interval * 1000;
			while (server.IsRunning())
			{
				Sleep(sleep);
				server.Update();
			}
		}

		bool __Initialize()
		{
			printf("Namiono - PXE Service Version 0.5\n");
			server.Add_Endpoint(DHCP_SERVER, UDP);
			server.Add_Endpoint(BOOTP_SERVER, UDP);
			server.Add_Endpoint(TFTP_SERVER, UDP);


			if (!server.Initialize())
			{
				printf("[E] Failed to initialize the Server!\n");
				return false;
			}

			return true;
		}

		int _main(int argc, const char * argv)
		{
			if (!__Initialize())
				return -1;

			std::thread _workerThread = std::thread(__Update, SETTINGS.SERVER_UPDATE_DELAY);
			_workerThread.detach();

			server.Listen(Handle_Request);

			server.Shutdown();

			return 0;
		}

		void Handle_Request(const ServiceType* servicetype, const std::string& ident,
			const sockaddr_in* remote, const char* buffer, const _SIZE_T length)
		{
			Packet* packet = new Packet(servicetype, buffer, &length);

			Client* c = server.Get_Client(*servicetype, ident, *remote);
			if (c == nullptr)
				c = server.Add_Client(*servicetype, remote);

			switch (packet->Get_Opcode())
			{
			case BINL_REQ:
				Add_Service(&server, BINL_SERVER)->Handle_Service_Request(&server, ident, c, packet);
				break;
			default:
				Add_Service(&server, *servicetype)->Handle_Service_Request(&server, ident, c, packet);
				break;
			}

			delete packet;
		}
	}
}


