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
#include "DHCP_Client.h"

namespace Namiono
{
	namespace Network
	{
		DHCP_Client::DHCP_Client(const std::string& nextIP)
		{
			this->wds = new WDS_Client();
			this->rbcp = new RBCP_Client();
			this->bsdp = new BSDP_Client();
			this->ipxe = new IPXE_Client();
			this->arch = new DHCP_ARCH(INTEL_X86);
			this->bootfile = new std::string("");
			this->prefix = new std::string("");
			this->vendorid = new DHCP_VENDOR(UNKNOWNNO);
			this->vendorstring = new std::string("");
			this->msgtype = new DHCP_MSGTYPE(OFFER);
			this->NextServer = new _IPADDR(inet_addr(nextIP.c_str()));
			this->isWDSRequest = new bool(false);
			this->isWDSResponse = new bool(false);
			this->isIPXERequest = new bool(false);
			this->isBSDPRequest = new bool(false);
			this->isRelayedPacket = new bool(false);
			this->state = new CLIENTSTATE(DHCP_INIT);
			this->vendorOpts = new std::vector<DHCP_Option>();
		}

		void DHCP_Client::SetNextServer(const _IPADDR& ip)
		{
			*this->NextServer = ip;
		}

		const _IPADDR& DHCP_Client::GetNextServer() const
		{
			return *this->NextServer;
		}

		void DHCP_Client::Set_State(const CLIENTSTATE& state)
		{
			*this->state = state;
		}

		const CLIENTSTATE& DHCP_Client::Get_State() const
		{
			return *this->state;
		}

		void DHCP_Client::Set_Vendor(const DHCP_VENDOR& vendor)
		{
			*this->vendorid = vendor;

			switch (*this->vendorid)
			{
			case PXEClient:
				Set_VendorString("PXEClient");
				SetIsBSDPRequest(false);
				break;
			case PXEServer:
				Set_VendorString("PXEServer");
				SetIsBSDPRequest(false);
				break;
			case AAPLBSDPC:
				Set_VendorString("AAPLBSDPC");
				SetIsBSDPRequest(true);
				break;
			}
		}

		const DHCP_VENDOR& DHCP_Client::Get_Vendor() const
		{
			return *this->vendorid;
		}

		void DHCP_Client::Set_VendorString(const std::string& vendor)
		{
			*this->vendorstring = vendor;
		}

		const std::string& DHCP_Client::Get_VendorString() const
		{
			return *this->vendorstring;
		}

		void DHCP_Client::SetMessageType(const DHCP_MSGTYPE& type)
		{
			*this->msgtype = type;
		}

		const DHCP_MSGTYPE& DHCP_Client::GetMessageType() const
		{
			return *this->msgtype;
		}

		void DHCP_Client::SetBootfile(const std::string& file)
		{
			*this->bootfile = file;
		}

		const std::string& DHCP_Client::GetPrefix() const
		{
			return *this->prefix;
		}

		void DHCP_Client::SetPrefix(const std::string& path)
		{
			*this->prefix = path;
		}

		const DHCP_ARCH& DHCP_Client::GetArchitecture() const
		{
			return *this->arch;
		}

		void DHCP_Client::SetArchitecture(const DHCP_ARCH& arch)
		{
			*this->arch = static_cast<DHCP_ARCH>(arch);
		}

		const bool DHCP_Client::GetIsWDSRequest() const
		{
			return *this->isWDSRequest;
		}

		const bool DHCP_Client::GetIsWDSResponse() const
		{
			return *this->isWDSResponse;
		}

		const bool DHCP_Client::GetIsIPXERequest() const
		{
			return *this->isIPXERequest;
		}

		void DHCP_Client::SetIsIPXERequest(bool is)
		{
			*this->isIPXERequest = is;
		}

		void DHCP_Client::SetIsWDSRequest(bool is)
		{
			*this->isWDSRequest = is;
		}

		void DHCP_Client::SetIsWDSResponse(bool is)
		{
			*this->isWDSResponse = is;
		}

		void DHCP_Client::SetIsBSDPRequest(bool is)
		{
			*this->isBSDPRequest = is;
		}

		const bool DHCP_Client::GetIsBSDPRequest() const
		{
			return *this->isBSDPRequest;
		}

		const bool DHCP_Client::GetIsRelayedPacket() const
		{
			return *this->isRelayedPacket;
		}

		void DHCP_Client::SetIsRelayedPacket(bool is)
		{
			*this->isRelayedPacket = is;
		}

		const std::string DHCP_Client::GetBootfile()
		{
			switch (this->GetArchitecture())
			{
			default:
			case INTEL_X86:
				*this->prefix = "Boot\\x86\\";
				this->wds->SetBCDfile(*this->prefix + "default.bcd");
				switch (this->wds->GetNextAction())
				{
				case ABORT:
					*this->bootfile = *this->prefix + "abortpxe.com";
					break;
				default:
					if (this->GetIsWDSRequest())
					{
						*this->bootfile = *this->prefix + "pxeboot.n12";
					}
					else
					{
						*this->bootfile = *this->prefix + "wdsnbp.com";
					}
					break;
				}
				break;
			case INTEL_IA32X64:
			case EFI_X86X64:
				*this->prefix = "Boot\\x64\\";
				this->wds->SetBCDfile(*this->prefix + "default.bcd");
				switch (this->wds->GetNextAction())
				{
				case ABORT:
					*this->bootfile = *this->prefix + "abortpxe.com";
					this->wds->SetBCDfile("");
					break;
				default:
					if (this->GetIsWDSRequest())
					{
						*this->bootfile = *this->prefix + "pxeboot.n12";
					}
					else
					{
						*this->bootfile = *this->prefix + "wdsnbp.com";

					}
					break;
				}
				break;
			case EFI_BC:
				*this->prefix = "Boot\\efi\\";
				this->wds->SetBCDfile(*this->prefix + "default.bcd");
				*this->bootfile = *this->prefix + "bootmgfw.efi";
				break;
			}

			return *this->bootfile;
		}

		std::vector<DHCP_Option>* DHCP_Client::Get_VendorOpts()
		{
			return this->vendorOpts;
		}

		WDS_Client * DHCP_Client::Get_WDSClient()
		{
			return this->wds;
		}

		RBCP_Client * DHCP_Client::Get_RBCPClient()
		{
			return this->rbcp;
		}

		IPXE_Client * DHCP_Client::Get_IPXEClient()
		{
			return this->ipxe;
		}

		BSDP_Client * DHCP_Client::Get_BSDPClient()
		{
			return this->bsdp;
		}

		DHCP_Client::~DHCP_Client()
		{
			delete this->wds;
			this->wds = nullptr;

			delete this->rbcp;
			this->rbcp = nullptr;

			delete this->ipxe;
			this->ipxe = nullptr;

			delete this->bsdp;
			this->bsdp = nullptr;

			delete this->arch;
			this->arch = nullptr;

			delete this->bootfile;
			this->bootfile = nullptr;

			delete this->msgtype;
			this->msgtype = nullptr;

			delete this->NextServer;
			this->NextServer = nullptr;

			delete this->isWDSRequest;
			this->isWDSRequest = nullptr;

			delete this->isWDSResponse;
			this->isWDSResponse = nullptr;

			delete this->isIPXERequest;
			this->isIPXERequest = nullptr;

			delete this->isRelayedPacket;
			this->isRelayedPacket = nullptr;

			delete this->state;
			this->state = nullptr;

			delete this->prefix;
			this->prefix = nullptr;

			delete this->vendorid;
			this->vendorid = nullptr;

			delete this->vendorstring;
			this->vendorstring = nullptr;

			this->vendorOpts->clear();

			delete this->vendorOpts;
			this->vendorOpts = nullptr;
		}
	}
}
