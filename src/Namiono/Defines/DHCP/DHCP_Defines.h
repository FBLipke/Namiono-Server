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
		typedef struct LeaseInfo
		{
			std::string mac;
			std::string addr;
		} LeaseInfo;

		typedef enum DHCP_RELAYAGENTOptions
		{
			CircuitID = 1,
			RemoteID = 2
		} DHCP_RELAYAGENTOptions;

		typedef enum DHCP_OPCODE
		{
			BOOTREQUEST = 0x01,
			BOOTREPLY = 0x02,
		} DHCP_OPCODE;

		typedef enum DHCP_MSGTYPE
		{
			DISCOVER = 0x01,
			OFFER = 0x02,
			REQUEST = 0x03,
			DECLINE = 0x04,
			ACK = 0x05,
			NAK = 0x06,
			RELEASE = 0x07,
			INFORM = 0x08
		} DHCP_MSGTYPE;

		typedef enum DHCP_VENDOR
		{
			UNKNOWNNO = 0x00,
			PXEClient = 0x01,
			PXEServer = 0x02,
			AAPLBSDPC = 0x03
		} DHCP_VENDOR;

		typedef enum DHCP_ARCH // LE
		{
			X86PC = 0x0000,
			NECPC98 = 0x0001,
			EFIItanium,
			DECAlpha,
			Arcx86,
			IntelLeanClient,
			EFI_IA32,
			EFIByteCode,
			EFI_xScale,
			EFI_x8664,
			ARM32_EFI,
			ARM64_EFI,
			PowerPCOpenFW,
			PowerPCePAPR,
			PowerOpalV3,
			X86EfiHttp,
			X64EfiHttp,
			EfiHttp,
			Arm32EfiHttp,
			Arm64EfiHttp,
			PCBiosHttp,
			Arm32Uboot,
			Arm64UBoot,
			Arm32UbootHttp,
			Arm64UbootHttp,
			RiscV32EFi,
			RiscV32EFiHttp,
			RiscV64EFi,
			RiscV64EFiHttp,
			RiscV128Efi,
			RiscV128EfiHttp,
			S390Basic,
			S390Extended,
			MIPS32Efi,
			MIPS64Efi,
			SunWay32Efi,
			SunWay64Efi,
			LoongArch32Efi,
			LoongArch32EfiHttp,
			LoongArch64Efi,
			LoongArch64EfiHttp,
			ArmRPIBoot
		} DHCP_ARCH;

		typedef enum DHCP_FLAGS
		{
			Unicast = 0x0000,
			Broadcast = 0x8000
		} DHCP_FLAGS;

		typedef enum DHCP_HARDWARETYPE
		{
			Unknown = 0x00,
			Ethernet = 0x01,
			Exp_Ethernet = 0x02,
			AX25 = 0x03,
			PRONET = 0x04,
			CHAOS = 0x05,
			IEEE802 = 0x06,
			ARCNET = 0x07,
			LocalTalk = 0x0b,
			LocalNet = 0x0c,
			SMDS = 0x0e,
			FrameRelay = 0x0f,
			ATM1 = 0x10,
			HDLC = 0x11,
			FireChannel = 0x12,
			ATM2 = 0x13,
			SerialLine = 0x14
		} DHCP_HARDWARETYPE;

		typedef struct DHCP_Option
		{
			DHCP_Option()
			{
				ClearBuffer(Value, Length);
			}

			~DHCP_Option() = default;

			DHCP_Option(const _BYTE& opt, const _BYTE& length, const void* value)
			{
				if (length > 255)
					return;

				Option = opt;
				Length = length;

				ClearBuffer(&Value, Length + static_cast<_BYTE>(1));

				if (length != 0)
					memcpy(&Value, value, Length);
			}

			DHCP_Option(const _BYTE& opt, const std::string& value)
			{
				if (value.size() > 255)
					return;

				Option = opt;
				Length = static_cast<_BYTE>(value.size());

				ClearBuffer(&Value, Length + static_cast<_BYTE>(1));

				if (Length != 0)
					memcpy(&Value, value.c_str(), Length);
			}

			DHCP_Option(const _BYTE& opt, const _BYTE& value)
			{
				Option = opt;
				Length = 1;

				ClearBuffer(&Value, Length + 1);

				if (Length != 0)
					memcpy(&Value, &value, Length);
			}

			DHCP_Option(const _BYTE& opt, const _USHORT& value)
			{
				Option = opt;
				Length = sizeof(_USHORT);

				ClearBuffer(&Value, Length + 1);

				if (Length != 0)
					memcpy(&Value, &value, Length);
			}

			DHCP_Option(const _BYTE& opt, const _ULONG& value)
			{
				Option = opt;
				Length = sizeof(_ULONG);

				ClearBuffer(&Value, Length + 1);

				if (Length != 0)
					memcpy(&Value, &value, Length);
			}

			DHCP_Option(const _BYTE& opt, const _BYTE length, const _ULONG& value)
			{
				Option = opt;
				Length = length;

				ClearBuffer(&Value, Length + 1);

				if (Length != 0)
					memcpy(&Value, &value, Length);
			}

			DHCP_Option(const _BYTE& opt, const _UINT& value)
			{
				Option = opt;
				Length = sizeof(_UINT);

				ClearBuffer(&Value, Length + 1);

				if (Length != 0)
					memcpy(&Value, &value, Length);
			}

			DHCP_Option(const _BYTE& opt, const std::vector<DHCP_Option>& value)
			{
				Option = opt;
				Length = 0;
				_INT32 offset = 0;

				// Get the entire option length!

				for (_SIZET i = 0; i < value.size(); i++)
					Length += value.at(i).Length + 2;

				if (Length > 255)
					return;

				ClearBuffer(&Value, Length + 1);

				for (_SIZET i = 0; i < value.size(); i++)
				{
					memcpy(&Value[offset], &value.at(i).Option, sizeof(_BYTE));
					offset += 1;

					memcpy(&Value[offset], &value.at(i).Length, sizeof(_BYTE));
					offset += 1;

					memcpy(&Value[offset], &value.at(i).Value, value.at(i).Length);
					offset += value.at(i).Length;
				}

				Length = offset;
			}

			DHCP_Option(const _BYTE& opt)
			{
				Option = opt;
				Length = 0;
				ClearBuffer(&Value, Length + 1);
			}

			std::string Get_Value_As_String() const
			{
				return std::string(this->Value);
			}

			_BYTE Get_Value_As_Byte() const
			{
				return static_cast<_BYTE>(this->Value[0]);
			}

			_USHORT Get_Value_As_USHORT()
			{
				_USHORT val = 0;
				memcpy(&val, &this->Value, sizeof(val));

				return static_cast<_USHORT>(val);
			}

			_IPADDR Get_Value_As_IPADDR()
			{
				_IPADDR val = 0;
				memcpy(&val, &this->Value, sizeof(val));

				return static_cast<_IPADDR>(val);
			}

			void Get_SubOptions(std::vector<DHCP_Option>& suboptionList)
			{
				suboptionList.clear();

				for (_SIZET i = 0; i < Length; i++)
				{
					if (static_cast<_BYTE>(Value[i]) == static_cast<_BYTE>(0xff) ||
						static_cast<_BYTE>(Value[i]) == static_cast<_BYTE>(0x00))
						break;

					suboptionList.emplace_back(static_cast<_BYTE>(Value[i]),
						static_cast<_BYTE>(Value[i + 1]), &Value[i + 2]);

					i += static_cast<_BYTE>(static_cast<_BYTE>(1) + static_cast<_BYTE>(Value[i + 1]));
				}
			}

			_BYTE Option = 0;
			_BYTE Length = 0;
			char Value[1024];
		} DHCP_Option;

		typedef struct DHCP_UPSTREAMSERVER
		{
			DHCP_UPSTREAMSERVER(const _IPADDR& address, const _USHORT& port)
			{
				this->address = address;
				this->port = port;
			}

			_USHORT Get_Port() const
			{
				return this->port;
			}

			_IPADDR Get_IPAddress() const
			{
				return this->address;
			}

			~DHCP_UPSTREAMSERVER()
			{

			}
		private:
			_USHORT port;
			_IPADDR address;

		} DHCP_UPSTREAMSERVER;


		typedef struct DHCP_RELAYSESSION
		{
			DHCP_RELAYSESSION(const _IPADDR& remote, const _IPADDR& relayIP, const _USHORT& iface)
			{
				this->iface = new _USHORT(iface);
				this->_remote = new _IPADDR(remote);
				this->_relayIP = new _IPADDR(relayIP);
			}

			~DHCP_RELAYSESSION()
			{
				delete this->_relayIP;
				this->_relayIP = nullptr;

				delete this->iface;
				this->iface = nullptr;

				delete this->_remote;
				this->_remote = nullptr;
			}

			_IPADDR Get_RelayIP()
			{
				return *this->_relayIP;
			}

			_IPADDR Get_RemoteIP()
			{
				return *this->_remote;
			}

			_USHORT Get_Interface()
			{
				return *this->iface;
			}

		private:
			_USHORT* iface = nullptr;
			_IPADDR* _remote = nullptr;
			_IPADDR* _relayIP = nullptr;
		} DHCP_RELAYSESSION;

		typedef struct DHCP_LEASE
		{
			std::string macaddress;
			std::map<_BYTE, DHCP_Option> options;

		} DHCP_LEASE;
	}
}
