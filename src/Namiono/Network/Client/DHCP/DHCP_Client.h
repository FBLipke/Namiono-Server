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

#include <Namiono/Namiono.h>
#pragma once

namespace Namiono
{
	namespace Network
	{
		class DHCP_Client
		{
		public:
			DHCP_Client(const std::string& nextIP);
			~DHCP_Client();

			void SetNextServer(const _IPADDR& ip);

			const _IPADDR& GetNextServer() const;

			void Set_State(const CLIENTSTATE& state);

			const CLIENTSTATE& Get_State() const;

			void Set_Vendor(const DHCP_VENDOR& vendor);

			const DHCP_VENDOR& Get_Vendor() const;
			void Set_VendorString(const std::string& vendor);
			const std::string& Get_VendorString() const;
			void SetMessageType(const DHCP_MSGTYPE& type);
			const DHCP_MSGTYPE& GetMessageType() const;

			void SetBootfile(const std::string& file);

			const std::string& GetPrefix() const;

			void SetPrefix(const std::string& path);

			const DHCP_ARCH& GetArchitecture() const;
			void SetArchitecture(const DHCP_ARCH& arch);

			const bool GetIsWDSRequest() const;

			const bool GetIsWDSResponse() const;

			const bool GetIsIPXERequest() const;

			void SetIsIPXERequest(bool is);

			void SetIsWDSRequest(bool is);

			void SetIsWDSResponse(bool is);

			void SetIsBSDPRequest(bool is);



			const bool GetIsBSDPRequest() const;

			const bool GetIsRelayedPacket() const;
			void SetIsRelayedPacket(bool is);
			const std::string GetBootfile();

			std::vector<DHCP_Option>* Get_VendorOpts();
			
			WDS_Client* Get_WDSClient();
			RBCP_Client* Get_RBCPClient();
			IPXE_Client* Get_IPXEClient();
			BSDP_Client* Get_BSDPClient();
		private:
			bool* isWDSRequest = nullptr;
			bool* isWDSResponse = nullptr;
			bool* isIPXERequest = nullptr;
			bool* isBSDPRequest = nullptr;
			bool* isRelayedPacket = nullptr;
			std::string* bootfile = nullptr;
			std::string* prefix = nullptr;
			DHCP_ARCH* arch = nullptr;
			DHCP_MSGTYPE* msgtype = nullptr;
			_BYTE* CircuitID = nullptr;
			_IPADDR* NextServer = nullptr;
			CLIENTSTATE* state = nullptr;
			DHCP_VENDOR* vendorid = nullptr;
			std::string* vendorstring = nullptr;
			WDS_Client* wds = nullptr;
			RBCP_Client* rbcp = nullptr;
			BSDP_Client* bsdp = nullptr;
			IPXE_Client* ipxe = nullptr;
			std::vector<DHCP_Option>* vendorOpts = nullptr;
		};
	}
}
