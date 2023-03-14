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

namespace Namiono
{
	namespace Network
	{
		Packet::Packet(const ServiceType& serviceType, const char* data, const _SIZET* length)
		{
			this->serviceType = serviceType;
			this->packetLength = *length;
			this->buffer = new char[*length];
			_USHORT tftp_op = 0;
			_UINT ris_opcode = 0;
			ClearBuffer(this->Get_Buffer(), *length);

			memcpy(this->Get_Buffer(), data, *length);

			switch (this->serviceType)
			{
			case ServiceType::DHCP_SERVER:
			case ServiceType::BINL_SERVER:
				if (this->get_opcode() == BOOTREQUEST)
					this->Set_Opcode(DHCP_REQ);

				if (this->get_opcode() == BOOTREPLY)
					this->Set_Opcode(DHCP_RES);

				switch (this->Get_Opcode())
				{
				case DHCP_REQ:
				case DHCP_RES:
					if (this->Get_Buffer() == nullptr)
					{
						print_Error("[E] FATAL ERROR: Have NULL Buffer!\n");
						return;
					}

					this->packetLength = *length;
					if (this->get_Length() <= static_cast<_SIZET>(240))
					{
						print_Error("[W] Got (very) short packet!\n");
						break;
					}

					for (_SIZET i = 240; i < this->get_Length(); i++)
					{
						if (static_cast<_BYTE>(this->Get_Buffer()[i]) == static_cast<_BYTE>(0xff))
							break;

						if (static_cast<_BYTE>(this->Get_Buffer()[i + 1]) == static_cast<_BYTE>(1))
						{
							Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(this->Get_Buffer()[i]),
								static_cast<_BYTE>(this->Get_Buffer()[i + sizeof(_USHORT)])));
						}
						else
						{
							Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(this->Get_Buffer()[i]),
								static_cast<_BYTE>(this->Get_Buffer()[i + 1]), &this->Get_Buffer()[i + sizeof(_USHORT)]));
						}

						i += static_cast<_SIZET>(1 + static_cast<_SIZET>(this->Get_Buffer()[i + 1]));
					}
					break;
				default:
					break;
				}
				break;
			case ServiceType::TFTP_SERVER:
				memcpy(&tftp_op, this->Get_Buffer(), sizeof(_USHORT));

				switch (static_cast<TFTP_OPCODE>(htons(tftp_op)))
				{
				case TFTP_OPCODE::TFTP_Read:
					this->Set_Opcode(TFTP_RRQ);
					char filename[255];
					ClearBuffer(&filename, sizeof filename);
					strncpy(filename, &this->Get_Buffer()[2], sizeof filename);

					this->Add_TFTPOption(TFTP_Option("File", std::string(filename)));

					for (const std::string& option : { "blksize", "tsize", "windowsize", "octet", "timeout", "msftwindow" })
					{
						std::stringstream sso;
						std::stringstream ssv;

						_SIZET position = 0;
						char* value = new char[64];
						ClearBuffer(value, 64);

						for (_SIZET i = sizeof(_USHORT); i < *length; i++)
						{
							_SIZET optlen = option.size();

							if (memcmp(option.c_str(), &this->Get_Buffer()[i], optlen) == 0)
							{
								position = static_cast<_SIZET>(i + (optlen + 1));

								strcpy(value, &this->Get_Buffer()[position]);
								sso << option;
								ssv << value;

								if (!Functions::Compare(ssv.str().c_str(), "octet", 5) && !Functions::Compare(sso.str().c_str(), "octet", 5))
									Add_TFTPOption(TFTP_Option(sso.str().c_str(), ssv.str().c_str()));
							}
						}

						delete[] value;
						value = nullptr;

						position = 0;
					}
					break;
				case TFTP_OPCODE::TFTP_Ack:
					this->Set_Opcode(TFTP_ACK);
					break;
				case TFTP_OPCODE::TFTP_Error:
					this->Set_Opcode(TFTP_ERR);
					break;
				}
				break;
			default:
				break;
			}
		}

		Packet::Packet(const ServiceType& serviceType, Packet& packet, const _SIZET& length, const DHCP_MSGTYPE& msgType)
		{
			_SIZET maxpktSize = length;
			this->serviceType = serviceType;

			if (packet.Has_DHCPOption(57))
				maxpktSize = packet.Get_DHCPOption(57)
				.Get_Value_As_USHORT();

			this->buffer = new char[length];
			ClearBuffer(this->Get_Buffer(), length);

			// Copy generic informations...

			this->Set_Opcode(DHCP_RES);
			set_hwtype(packet.get_hwtype());
			set_hwlength(packet.get_hwlength());

			memcpy(&this->Get_Buffer()[28], &packet.Get_Buffer()[28], this->get_hwlength());

			if (packet.get_xid() == 0)
			{
				printf("[W] DHCP client (%s): Invalid Transaction ID (%ld)!\n",
					Functions::AddressStr(packet.get_clientIP()).c_str(), packet.get_xid());
			}


			set_xid(packet.get_xid());

			switch (get_flags())
			{
			case DHCP_FLAGS::Unicast:
					set_flags(get_clientIP() == 0 ? DHCP_FLAGS::Broadcast : packet.get_flags());
				break;
			case DHCP_FLAGS::Broadcast:
				set_flags(packet.get_flags());
				break;
			default:
				break;
			}

			set_hops(packet.get_hops());
			set_secs(packet.get_secs());
			set_clientIP(packet.get_clientIP());
			set_yourIP(packet.get_yourIP());
			set_relayIP(packet.get_relayIP());
			set_nextIP(packet.get_nextIP());

			set_cookie();
			packetLength = 240;

			Add_DHCPOption(DHCP_Option(53, static_cast<_BYTE>(msgType)));

			
			if (packet.Has_DHCPOption(97))
				Add_DHCPOption(packet.Get_DHCPOption(97));

			if (Has_DHCPOption(50))
				Remove_DHCPOption(50);
		}

		Packet::Packet(const ServiceType& serviceType, Packet& packet, const _SIZET& length)
		{
			_SIZET maxpktSize = length + static_cast<_SIZET>(1024);
			this->serviceType = serviceType;

			this->buffer = new char[maxpktSize];
			ClearBuffer(this->Get_Buffer(), maxpktSize);

			switch (packet.get_opcode())
			{
			case BOOTREPLY:
				this->Set_Opcode(DHCP_RES);
				break;
			case BOOTREQUEST:
				this->Set_Opcode(DHCP_REQ);
				break;
			default:
				break;
			}
			
			memcpy(&this->Get_Buffer()[0], &packet.Get_Buffer()[0], packet.get_Length());
			this->packetLength = maxpktSize;

			for (_SIZET i = 240; i < this->get_Length(); i++)
			{
				if (static_cast<_BYTE>(this->Get_Buffer()[i]) == static_cast<_BYTE>(0xff))
					break;

				if (static_cast<_BYTE>(this->Get_Buffer()[i + 1]) == static_cast<_BYTE>(1))
				{
					Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(this->Get_Buffer()[i]),
						static_cast<_BYTE>(this->Get_Buffer()[i + sizeof(_USHORT)])));
				}
				else
				{
					Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(this->Get_Buffer()[i]),
						static_cast<_BYTE>(this->Get_Buffer()[i + 1]), &this->Get_Buffer()[i + sizeof(_USHORT)]));
				}

				i += static_cast<_SIZET>(1 + static_cast<_SIZET>(this->Get_Buffer()[i + 1]));
			}
		}


		Packet::Packet(const ServiceType& serviceType, const _SIZET& length, const Packet_OPCode& opcode)
		{
			this->buffer = new char[length];
			ClearBuffer(this->Get_Buffer(), length);
			this->serviceType = serviceType;
			this->set_Length(length);
			this->Set_Opcode(opcode);
		}

		Packet::~Packet()
		{
			delete[] this->buffer;
			this->buffer = nullptr;

			dhcp_options.clear();

			tftp_options.clear();
		}

		_SIZET Packet::Write(const void* data, const _SIZET& length, const _SIZET& position)
		{
			memcpy(&this->Get_Buffer()[position], data, length);
			this->set_Length(length);

			return length;
		}

		void Packet::Write(const _ULONG& data, const _SIZET& position)
		{
			memcpy(&this->Get_Buffer()[position], &data, sizeof(_ULONG));
			this->set_Length(sizeof(_ULONG));
		}

		void Packet::Write(const _UINT& data, const _SIZET& position)
		{
			memcpy(&this->Get_Buffer()[position], &data, sizeof(_UINT));
			this->set_Length(sizeof(_UINT));
		}

		void Packet::Write(const _BYTE& data, const _SIZET& position)
		{
			memcpy(&this->Get_Buffer()[position], &data, sizeof(_BYTE));
			this->set_Length(sizeof(_BYTE));
		}

		void Packet::Write(const _USHORT& data, const _SIZET& position)
		{
			memcpy(&this->Get_Buffer()[position], &data, sizeof(_USHORT));
		}

		_SIZET Packet::Read(void* data, const _SIZET& length, const _SIZET& position)
		{
			memcpy(data, &this->Get_Buffer()[position], length);
			return length;
		}

		void Packet::Add_DHCPOption(const DHCP_Option& option)
		{
			Remove_DHCPOption(option.Option);

			dhcp_options.emplace(option.Option, option);
		}

		void Packet::Add_TFTPOption(const TFTP_Option& option)
		{
			Remove_TFTPOption(option.Option);

			tftp_options.emplace(option.Option, option);
		}

		void Packet::Remove_DHCPOption(const _BYTE& opt)
		{
			dhcp_options.erase(opt);
		}

		void Packet::Remove_TFTPOption(const std::string& opt)
		{
			if (Has_TFTPOption(opt))
				tftp_options.erase(opt);
		}

		bool Packet::Has_DHCPOption(const _BYTE& option)
		{
			return dhcp_options.find(option) != dhcp_options.end();
		}

		bool Packet::Has_TFTPOption(const std::string& option)
		{
			return tftp_options.find(option) != tftp_options.end();
		}

		void Packet::Set_Block(const _USHORT block)
		{
			_USHORT blk = htons(block);
			memcpy(&this->Get_Buffer()[sizeof(_USHORT)],
				&blk, sizeof(_USHORT));
		}

		_USHORT Packet::Get_Block()
		{
			_USHORT blk = 0;
			memcpy(&blk, &this->Get_Buffer()[sizeof(_USHORT)], sizeof blk);
			return htons(blk);
		}

		void Packet::Dump()
		{
			switch (this->Get_Opcode())
			{
			case BINL_RSU:
				FileWrite("RSU_Packet_DUMP.hex", this->Get_Buffer(), this->get_Length());
				break;
			case BINL_CHA:
				FileWrite("CHA_Packet_DUMP.hex", this->Get_Buffer(), this->get_Length());
				break;
			case BINL_RQU:
				FileWrite("RQU_Packet_DUMP.hex", this->Get_Buffer(), this->get_Length());
				break;
			default:
				break;
			}
		}

		void Packet::Trim()
		{
			switch (this->Get_Opcode())
			{
			case DHCP_RES:
			case DHCP_REQ:
				this->packetLength = (Functions::Strip(this->Get_Buffer(), this->get_Length()));
				break;
			case TFTP_OACK:
			case TFTP_RRQ:
				this->packetLength = (Functions::Strip(this->Get_Buffer(), this->get_Length()) - 1);
				break;
			default:
				return;
			}
		}

		void Packet::Commit()
		{
			/*
				Reset the Length of the packet to its minimal size and start commit. At
				the End of the Commit, strip the packet. To the Last position of 0xFF;
				In DHCP packets it is the end Option.
			*/

			_SIZET _offset = 0;
			_SIZET innerlen = 0;

			switch (this->Get_Opcode())
			{
			case DHCP_RES:
			case DHCP_REQ:
				Add_DHCPOption(DHCP_Option(static_cast<_BYTE>(0xff)));
				packetLength = 240;
				_offset = packetLength;

				for (const std::pair<_BYTE, DHCP_Option>& option : dhcp_options)
				{
					_offset += Write(&option.second.Option, sizeof(_BYTE), _offset);
					_offset += Write(&option.second.Length, sizeof(_BYTE), _offset);

					if (option.second.Length != 1)
						_offset += Write(&option.second.Value, option.second.Length, _offset);
					else if (option.second.Length != 0)
						_offset += Write(&option.second.Value, option.second.Length, _offset);
				}

				this->Trim();
				break;
			case TFTP_OACK:
				/*
					Set an DHCP style like marker to the end of the options and strip the packet!
					The Marker gets removed while we strip the packet! by removing the marker LENGTH - 1!
				*/

				Add_TFTPOption(TFTP_Option(0xff));
				packetLength = 2;

				_offset = packetLength;

				for (const std::pair<const std::string, TFTP_Option> & option : tftp_options)
				{
					_offset += Write(option.first.c_str(), static_cast<_SIZET>(option.first.size() + 1), _offset);
					_offset += static_cast<_SIZET>(Write(&option.second.Value, option.second.Length, _offset) + 1);

					this->set_Length(_offset);
				}

				this->Trim();
				break;
			case TFTP_ERR:
				break;
			case BINL_RSU:
			case BINL_CHA:
				innerlen = (this->get_Length() - 8);
				Write(&innerlen, sizeof innerlen, 4);
				break;
			default:
				break;
			}
		}

		const Packet_OPCode& Packet::Get_Opcode() const
		{
			return this->opcode;
		}

		char* Packet::Get_Buffer() const
		{
			return this->buffer;
		}

		void Packet::Set_Opcode(const Packet_OPCode& op)
		{
			this->opcode = op;

			switch (this->opcode)
			{
			case DHCP_REQ:
				this->set_opcode(BOOTREQUEST);
				break;
			case DHCP_RES:
				this->set_opcode(BOOTREPLY);
				this->set_cookie();
				break;
			case TFTP_DAT:
				Write(static_cast<_USHORT>(htons(3)), 0);
				break;
			case TFTP_RRQ:
				Write(static_cast<_USHORT>(htons(1)), 0);
				break;
			case TFTP_ACK:
				Write(static_cast<_USHORT>(htons(4)), 0);
				break;
			case TFTP_ERR:
				Write(static_cast<_USHORT>(htons(5)), 0);
				break;
			case TFTP_OACK:
				Write(static_cast<_USHORT>(htons(6)), 0);
				break;
			case BINL_RQU:
				Write(static_cast<_ULONG>(BINL_RQU), 0);
				break;
			case BINL_RSU:
				Write(static_cast<_ULONG>(BINL_RQU), 0);
				break;
			case BINL_CHA:
				Write(static_cast<_ULONG>(BINL_CHA), 0);
				break;
			case BINL_REQ:
				break;
			default:
				printf("[E] Packet (Set_OPCode): Unknown OPCode: %d\n", this->opcode);
				break;
			}
		}

		void Packet::set_opcode(const DHCP_OPCODE& op)
		{
			Write(static_cast<_BYTE>(op), 0);
		}

		DHCP_OPCODE Packet::get_opcode()
		{
			_BYTE _tmp = 0;
			Read(&_tmp, sizeof(_BYTE), 0);

			return static_cast<DHCP_OPCODE>(_tmp);
		}

		void Packet::set_hwtype(const DHCP_HARDWARETYPE& hwtype)
		{
			Write(static_cast<_BYTE>(hwtype), 1);
		}

		DHCP_HARDWARETYPE Packet::get_hwtype()
		{
			_BYTE _tmp = 0;
			Read(&_tmp, sizeof(_BYTE), 1);

			return static_cast<DHCP_HARDWARETYPE>(_tmp);
		}

		void Packet::set_hwlength(const _BYTE& length)
		{
			Write(static_cast<_BYTE>(length), 2);
		}

		_BYTE Packet::get_hwlength()
		{
			_BYTE _tmp = 0;
			Read(&_tmp, sizeof(_BYTE), 2);

			return _tmp;
		}

		void Packet::set_hops(const _BYTE& hops)
		{
			Write(static_cast<_BYTE>(hops), 3);
		}

		void Packet::increase_hops(const _BYTE & hops)
		{
			set_hops(get_hops() + 1);
		}

		_BYTE Packet::get_hops()
		{
			_BYTE _tmp = 0;
			Read(&_tmp, sizeof(_BYTE), 3);

			return _tmp;
		}

		void Packet::set_xid(const _ULONG& xid)
		{
			Write(static_cast<_ULONG>(xid), 4);
		}

		_ULONG Packet::get_xid()
		{
			_ULONG _tmp = 0;
			Read(&_tmp, sizeof(_ULONG), 4);

			return _tmp;
		}

		void Packet::set_secs(const _USHORT& secs)
		{
			Write(static_cast<_USHORT>(secs), 8);
		}

		_USHORT Packet::get_secs()
		{
			_USHORT _tmp = 0;
			Read(&_tmp, sizeof(_USHORT), 8);

			return _tmp;
		}

		void Packet::set_flags(const DHCP_FLAGS& flags)
		{
			Write(static_cast<_USHORT>(htons(flags)), 10);
		}

		DHCP_FLAGS Packet::get_flags()
		{
			_USHORT _tmp = 0;
			Read(&_tmp, sizeof(_USHORT), 10);

			return static_cast<DHCP_FLAGS>(_tmp);
		}

		void Packet::set_clientIP(const _IPADDR& ip)
		{
			Write(&ip, sizeof(_IPADDR), 12);
		}

		_IPADDR Packet::get_clientIP()
		{
			_IPADDR _tmp = 0;
			Read(&_tmp, sizeof(_IPADDR), 12);

			return _tmp;
		}

		void Packet::set_yourIP(const _IPADDR& ip)
		{
			Write(&ip, sizeof(_IPADDR), 16);
		}

		_IPADDR Packet::get_yourIP()
		{
			_IPADDR _tmp = 0;
			Read(&_tmp, sizeof(_IPADDR), 16);

			return _tmp;
		}

		void Packet::set_nextIP(const _IPADDR& ip)
		{
			Write(&ip, sizeof(_IPADDR), 20);
		}

		_IPADDR Packet::get_nextIP()
		{
			_IPADDR _tmp = 0;
			Read(&_tmp, sizeof(_IPADDR), 20);

			return _tmp;
		}

		void Packet::set_relayIP(const _IPADDR& ip)
		{
			Write(&ip, sizeof(_IPADDR), 24);
		}

		_IPADDR Packet::get_relayIP()
		{
			_IPADDR _tmp = 0;
			Read(&_tmp, sizeof(_IPADDR), 24);

			return _tmp;
		}

		void Packet::set_hwaddress(const void* mac)
		{
			Write(&mac, 16, 28);
		}

		void Packet::get_hwaddress(char* out, _SIZET length)
		{
			ClearBuffer(out, length);

			Read(out, length, 28);
		}

		std::string Packet::get_hwaddress()
		{
			_SIZET _length = this->get_hwlength();
			char* out = new char[_length];
			ClearBuffer(out, _length);
			std::string tmp = "";

			
			Read(out, _length, 28);
			
			tmp = Functions::MacAsString(out);
			
			delete[] out;
			out = nullptr;

			return tmp;
		}

		void Packet::set_servername(const std::string& sname)
		{
			ClearBuffer(&this->Get_Buffer()[44], 64);
			Write(sname.c_str(), static_cast<_SIZET>(sname.size()), 44);
		}

		std::string Packet::get_servername()
		{
			char _tmp[64];
			ClearBuffer(_tmp, sizeof _tmp);

			Read(&_tmp, sizeof _tmp, 44);

			return std::string(_tmp);
		}

		void Packet::set_filename(const std::string& file)
		{
			ClearBuffer(&this->Get_Buffer()[108], 128);
			Write(file.c_str(), static_cast<_SIZET>(file.size()), 108);
		}

		std::string Packet::get_filename()
		{
			char _tmp[128];
			ClearBuffer(_tmp, sizeof _tmp);

			Read(&_tmp, sizeof _tmp, 108);

			return std::string(_tmp);
		}

		void Packet::set_cookie()
		{
			this->Get_Buffer()[236] = static_cast<_BYTE>(0x63);
			this->Get_Buffer()[237] = static_cast<_BYTE>(0x82);
			this->Get_Buffer()[238] = static_cast<_BYTE>(0x53);
			this->Get_Buffer()[239] = static_cast<_BYTE>(0x63);
		}

		_ULONG Packet::get_cookie()
		{
			_ULONG _tmp = 0;

			Read(&_tmp, sizeof(_ULONG), 236);

			return _tmp;
		}

		std::string Packet::get_osc_filename(const std::string& rootDir)
		{
			_SIZET startpos = 28;

			_SIZET length = this->get_Length() - (startpos + 1);
			std::string filename = "";


			if (length == 0)
			{
				filename = "welcome.osc";
			}
			else
			{
				char* filname = new char[length];
				ClearBuffer(filname, length);

				memcpy(&filname, &this->Get_Buffer()[startpos], length);

				filename = std::string(filname + std::string(".osc"));
				delete[] filname;
				filname = nullptr;
			}

			return filename;
		}

		void Packet::CopyFrom(const Packet & src, const _SIZET& srcOffset, const _SIZET& dstOffset, const _SIZET& length)
		{
			memcpy(&this->Get_Buffer()[dstOffset], &src.Get_Buffer()[srcOffset], length);
		}

		void Packet::set_Length(const _SIZET& length)
		{
			this->packetLength += length;
		}

		_SIZET Packet::get_Length() const
		{
			return this->packetLength;
		}

		DHCP_Option Packet::Get_DHCPOption(const _BYTE& option) const
		{
			return dhcp_options.at(option);
		}

		TFTP_Option Packet::Get_TFTPOption(const std::string& option) const
		{
			return this->tftp_options.at(option);
		}
	}
}
