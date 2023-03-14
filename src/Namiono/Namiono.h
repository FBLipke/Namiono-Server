/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#ifndef NAMIONO_NAMIONO_H_
#define NAMIONO_NAMIONO_H_

#include <Environment/environment.h>

#include <Namiono/Defines/Namiono_Defines.h>
#include <Namiono/Defines/DHCP/RBCP/RBCP_Defines.h>
#include <Namiono/Defines/DHCP/BSDP/BSDP_Defines.h>
#include <Namiono/Defines/DHCP/IPXE/IPXE_Defines.h>
#include <Namiono/Defines/DHCP/WDS/WDS_Defines.h>
#include <Namiono/Defines/DHCP/DHCP_Defines.h>
#include <Namiono/Defines/TFTP/TFTP_Defines.h>

#include <Namiono/Common/Functions.h>
#include <Namiono/Common/Filesystem.h>

using namespace Namiono::Common;

#include <Namiono/Network/Packet/Packet.h>

#include <Namiono/Network/Client/DHCP/WDS/WDS_Client.h>
#include <Namiono/Network/Client/DHCP/BSDP/BSDP_Client.h>
#include <Namiono/Network/Client/DHCP/RBCP/RBCP_Client.h>
#include <Namiono/Network/Client/DHCP/IPXE/IPXE_Client.h>
#include <Namiono/Network/Client/DHCP/DHCP_Client.h>
#include <Namiono/Network/Client/TFTP/TFTP_Client.h>
#include <Namiono/Network/Client/HTTP/HTTP_Client.h>

#include <Namiono/Network/Client/Client.h>

#include <Namiono/Network/Server/Interface.h>

#include <Namiono/Network/Server/Server.h>

#include <Namiono/Services/Service.h>
#include <Namiono/Services/DHCP/DHCP_Functions.h>
#include <Namiono/Services/DHCP/DHCP_Service.h>
#include <Namiono/Services/DHCP/ProxyDHCP_Service.h>

#include <Namiono/Services/TFTP/TFTP_Service.h>
#include <Namiono/Network/Network.h>

namespace Namiono
{
	class _Namiono
	{
	public:
		_Namiono() {};
		explicit _Namiono(_INT32 argc, char* argv[]);
		~_Namiono();

		bool Init();
		void Start();
		void Process();
		void Heartbeat();
		void Close();

		SETTINGS* Get_Settings();

	private:
		SETTINGS* settings;

		std::string TFTPRootDir;
		Namiono::Network::Network* network = nullptr;
	};
}
#endif
