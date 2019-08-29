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
#include <Environment/Defines.h>
#include <Namiono/Common/Functions.h>
#include <Namiono/Common/Filesystem.h>

using namespace Namiono::Common;

#include <Namiono/Network/Packet/Packet.h>
#include <Namiono/Network/Client/Client.h>
#include <Namiono/Network/Server/Interface.h>
#include <Namiono/Network/Server/Server.h>

#include <Namiono/Services/Service.h>
#include <Namiono/Services/BootService.h>

#include <Namiono/Services/DHCP/DHCP_Functions.h>
#include <Namiono/Services/DHCP/DHCP_Service.h>
#include <Namiono/Services/DHCP/ProxyDHCP_Service.h>

#include <Namiono/Services/TFTP/TFTP_Service.h>
#include <Namiono/Network/Network.h>

namespace Namiono
{
	static std::string TFTPRootDir = Combine(CurrentDirectory(), "TFTP_Root");
	void Bootstrap();
}
#endif
