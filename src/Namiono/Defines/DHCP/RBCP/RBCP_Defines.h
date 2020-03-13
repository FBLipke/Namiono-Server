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

#include <Environment/environment.h>
#pragma once
namespace Namiono
{
	namespace Network
	{
		typedef enum RBCP_DISCOVERYCONTROL
		{
			DISABLE_BCAST = 1,
			DISABLE_MCAST = 2,
			UNICAST_ONLY = DISABLE_BCAST | DISABLE_MCAST,
			SERVERLIST_ONLY = 4,
			BCAST_SERVERLIST = DISABLE_MCAST | SERVERLIST_ONLY,
			UNICAST_SERVERLIST = UNICAST_ONLY | SERVERLIST_ONLY,
			BSTRAP_OVERRIDE = 8

		} RBCP_DISCOVERYCONTROL;

		typedef enum BootServerType
		{
			PXEBootstrapServer = 0x0000,
			WindowsNTBootServer = 0x0001,
			IntelLCMBootServer = 0x0002,
			PXEAPITestServer = 0xffff
		} BootServerType;

		typedef enum BootServerReplyType
		{
			Boot = 0x0000,
			Auth = 0x0001
		} BootServerReplyType;

		typedef struct BootServerEntry
		{
			std::vector<_IPADDR> Addresses;
			std::string Description = "";
			std::string Bootfile = "";

			_USHORT Ident = 0;

			BootServerEntry()
			{
				ClearBuffer(this, sizeof(*this));
			}

			BootServerEntry(const _USHORT& id, const std::string& desc, const std::vector<_IPADDR>& adresses,
				const std::string& filename = "")
			{
				Ident = id;
				Description = desc;
				Bootfile = filename;
				Addresses = adresses;
			}

			~BootServerEntry()
			{
			}

		} BootServerEntry;

		typedef struct BootMenuEntry
		{
			BootServerType Type = PXEBootstrapServer;
			std::string Description = "";
			_USHORT Ident = 0;
			_BYTE DescLength = 0;
			_IPADDR Address = 0;

			BootMenuEntry(const _USHORT& id, const std::string& text,
				const BootServerType type = BootServerType::PXEBootstrapServer)
			{
				Ident = id;
				Type = type;
				Description = text;
				DescLength = static_cast<_BYTE>(Description.size());
			}

			BootMenuEntry(const _USHORT& id, const std::string& text, const _IPADDR& address,
				const BootServerType type = BootServerType::PXEBootstrapServer)
			{
				Ident = id;
				Type = type;
				Address = address;
				Description = text;
				DescLength = static_cast<_BYTE>(Description.size());
			}

			~BootMenuEntry()
			{

			}

			BootMenuEntry()
			{
				ClearBuffer(this, sizeof *this);
			}
		} BootMenuEntry;

		typedef enum RBCP_LAYER
		{
			Bootfile = 0x0000,
			Credentials = 0x0001

		} RBCP_LAYER;

		typedef enum RBCP_Options
		{
			PXE_MTFTP_IP_ADDR = 1,
			PXE_MTFTP_CLIENT_PORT = 2,
			PXE_MTFTP_SERVER_PORT = 3,
			PXE_MTFTP_START_DELAY = 4,
			PXE_MTFTP_TIMEOUT = 5,
			PXE_DISCOVERY_CONTROL = 6,
			PXE_DISCOVERY_MCADDR = 7,
			PXE_BOOT_SERVERS = 8,
			PXE_BOOT_MENU = 9,
			PXE_MENU_PROMPT = 10,
			PXE_CREDENTIAL_TYPES = 12,
			PXE_NIC_PATH = 64,
			PXE_MAN_INFO = 65,
			PXE_OS_INFO = 66,
			PXE_BOOT_OS_INFO = 67,
			PXE_PROMPT_INFO = 68,
			PXE_OS_INFO2 = 69,
			PXE_BOOT_OS_INFO2 = 70,
			PXE_BOOT_ITEM = 71,

			PXE_LCM_SERVER = 179,
			PXE_LCM_DOMAIN = 180,
			PXE_LCM_NIC_OPT0 = 181,
			PXE_LCM_WORKGROUP = 190,
			PXE_LCM_DISCOVERY = 191,
			PXE_LCM_CONFIGURED = 192,
			PXE_LCM_VERSION = 193,
			PXE_LCM_SERIALNO = 194,
		} RBCP_Options;
	}
}
