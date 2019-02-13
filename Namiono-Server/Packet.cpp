#include <Namiono-Server.h>

EXPORT Packet::Packet(const ServiceType* serviceType, const char* data, const _SIZE_T* length)
{
	this->serviceType = *serviceType;
	this->packetLength = 0;
	this->buffer = new char[*length];
	unsigned short tftp_op = 0;

	ClearBuffer(this->Get_Buffer(), *length);

	memcpy(this->Get_Buffer(), data, *length);

	switch (this->serviceType)
	{
	case ServiceType::DHCP_SERVER:
	case ServiceType::BOOTP_SERVER:
		if (this->get_opcode() == BOOTREQUEST)
			this->Set_Opcode(DHCP_REQ);

		if (this->get_opcode() == BINL_REQUEST)
		{
			this->Set_Opcode(BINL_REQ);
			this->packetLength = this->Get_Buffer()[4];
		}

		if (this->get_opcode() == BINL_REPLY)
		{
			this->Set_Opcode(BINL_RES);
			this->packetLength = this->Get_Buffer()[4];
		}

		if (this->get_opcode() == BOOTREPLY)
			this->Set_Opcode(DHCP_RES);

		switch (this->Get_Opcode())
		{
		case DHCP_REQ:
		case DHCP_RES:
			if (this->Get_Buffer() == nullptr)
			{
				printf("FATAL ERROR: Have NULL Buffer\n!");
				return;
			}

			this->packetLength = *length;
			if (this->get_Length() <= static_cast<_SIZE_T>(240))
				break;

			for (_SIZE_T i = 240; i < this->get_Length(); i++)
			{
				if (static_cast<unsigned char>(this->Get_Buffer()[i]) == static_cast<unsigned char>(0xff))
					break;

				if (static_cast<unsigned char>(this->Get_Buffer()[i + 1]) == static_cast<unsigned char>(1))
				{
					auto x = this->Get_Buffer()[i];
					Add_DHCPOption(DHCP_Option(static_cast<unsigned char>(x),
						static_cast<unsigned char>(this->Get_Buffer()[i + 2])));
				}
				else
				{
					Add_DHCPOption(DHCP_Option(static_cast<unsigned char>(this->Get_Buffer()[i]),
						static_cast<unsigned char>(this->Get_Buffer()[i + 1]), &this->Get_Buffer()[i + 2]));
				}

				i += 1 + this->Get_Buffer()[i + 1];
			}
			break;
		default:
			break;
		}
		break;
	case ServiceType::TFTP_SERVER:
		memcpy(&tftp_op, this->Get_Buffer(), 2);

		switch (static_cast<TFTP_OPCODE>(BS16(tftp_op)))
		{
		case TFTP_OPCODE::TFTP_Read:
			this->Set_Opcode(TFTP_RRQ);
			char filename[255];
			ClearBuffer(&filename, sizeof filename);
			strncpy(filename, &this->Get_Buffer()[2], sizeof filename);

			this->Add_TFTPOption(TFTP_Option("File", std::string(filename)));

			for (auto & option : { "blksize", "tsize", "windowsize", "octet", "timeout", "msftwindow" })
			{
				std::stringstream* sso = new std::stringstream;
				std::stringstream* ssv = new std::stringstream;

				_SIZE_T position = 0;
				char value[64];
				ClearBuffer(value, 64);

				for (_SIZE_T i = 2; i < *length; i++)
					if (memcmp(option, &this->Get_Buffer()[i], strlen(option)) == 0)
					{
						printf("option \"%s\" in packet!\n", option);
						position = static_cast<_SIZE_T>(i + (strlen(option) + 1));

						strcpy(value, &this->Get_Buffer()[position]);
						*sso << option;
						*ssv << value;

						if (Functions::Compare(ssv->str().c_str(), "octet", 5))
							Add_TFTPOption(TFTP_Option("Mode", "octet"));
						else
							Add_TFTPOption(TFTP_Option(sso->str().c_str(), ssv->str().c_str()));
					}
				position = 0;

				delete sso;
				delete ssv;
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
	case BINL_REQUEST:
	case BINL_REPLY:
		this->packetLength = this->Get_Buffer()[4];
		if (this->get_opcode() == BINL_REQUEST)
			this->Set_Opcode(BINL_REQ);

		if (this->get_opcode() == BINL_REPLY)
			this->Set_Opcode(BINL_RES);
		break;
	}
}

EXPORT Packet::Packet(Packet& packet, const _SIZE_T length, DHCP_MSGTYPE msgType)
{
	_SIZE_T maxpktSize = length;

	if (packet.Has_DHCPOption(57))
		memcpy(&maxpktSize, &packet.Get_DHCPOption(57).Value[0], 2);
	this->buffer = new char[length];
	ClearBuffer(this->Get_Buffer(), length);

	// Copy generic informations...

	this->Set_Opcode(DHCP_RES);
	set_hwtype(packet.get_hwtype());
	set_hwlength(packet.get_hwlength());

	memcpy(&this->Get_Buffer()[28], &packet.Get_Buffer()[28], this->get_hwlength());
	set_xid(packet.get_xid());
	set_flags(packet.get_flags());
	set_hops(packet.get_hops());
	set_secs(packet.get_secs());
	set_clientIP(packet.get_clientIP());
	set_yourIP(packet.get_yourIP());
	set_relayIP(packet.get_relayIP());
	set_nextIP(packet.get_nextIP());

	set_cookie();
	packetLength = 240;

	Add_DHCPOption(DHCP_Option(53, static_cast<unsigned char>(msgType)));
	Add_DHCPOption(DHCP_Option(60, 9, "PXEClient"));

	if (packet.Has_DHCPOption(97))
		Add_DHCPOption(packet.Get_DHCPOption(97));
}

EXPORT Packet::Packet(const _SIZE_T length, Packet_OPCode opcode)
{
	this->buffer = new char[length];
	ClearBuffer(this->Get_Buffer(), length);
	this->set_Length(length);
	this->Set_Opcode(opcode);

}

EXPORT Packet::~Packet()
{
	delete[] buffer;
	dhcp_options.clear();
	tftp_options.clear();
}

EXPORT INLINE _SIZE_T Packet::Write(const void* data, const _SIZE_T length, const _SIZE_T position)
{
	memcpy(&this->Get_Buffer()[position], data, length);
	this->set_Length(length);

	return length;
}

EXPORT INLINE void Packet::Write(unsigned int data, const _SIZE_T position)
{
	memcpy(&this->Get_Buffer()[position], &data, sizeof(unsigned int));
	this->set_Length(sizeof(unsigned int));
}

EXPORT INLINE void Packet::Write(unsigned char data, const _SIZE_T position)
{
	memcpy(&this->Get_Buffer()[position], &data, sizeof(unsigned char));
	this->set_Length(sizeof(unsigned char));
}

EXPORT INLINE void Packet::Write(unsigned short data, const _SIZE_T position)
{
	memcpy(&this->Get_Buffer()[position], &data, sizeof(unsigned short));
}

EXPORT INLINE _SIZE_T Packet::Read(void* data, const _SIZE_T length, const _SIZE_T position)
{
	memcpy(data, &this->Get_Buffer()[position], length);
	return length;
}

EXPORT void Packet::Add_DHCPOption(const DHCP_Option option)
{
	Remove_DHCPOption(option.Option);

	dhcp_options.insert(std::pair<unsigned char, DHCP_Option>(option.Option, option));
}

EXPORT void Packet::Add_TFTPOption(const TFTP_Option option)
{
	Remove_TFTPOption(option.Option);

	tftp_options.insert(std::pair<std::string, TFTP_Option>(option.Option, option));
}

void Packet::Remove_DHCPOption(const unsigned char& opt)
{
	if (Has_DHCPOption(opt))
		dhcp_options.erase(opt);
}

void Packet::Remove_TFTPOption(const std::string& opt)
{
	if (Has_TFTPOption(opt))
		tftp_options.erase(opt);
}

EXPORT INLINE bool Packet::Has_DHCPOption(const unsigned char& option)
{
	return dhcp_options.find(option) != dhcp_options.end();
}

EXPORT INLINE bool Packet::Has_TFTPOption(const std::string& option)
{
	return tftp_options.find(option) != tftp_options.end();
}

EXPORT void Packet::Set_Block(const unsigned short block)
{
	unsigned short blk = block;
	memcpy(&this->Get_Buffer()[2], &blk, 2);
}

unsigned short Packet::Get_Block()
{
	unsigned short blk = 0;
	memcpy(&blk, &this->Get_Buffer()[2], sizeof blk);
	return BS16(blk);
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
	}
}

EXPORT void Packet::Commit()
{
	/*
		Reset the Length of the packet to its minimal size and start commit. At
		the End of the Commit, strip the packet. To the Last position of 0xFF;
		In DHCP packets it is the end Option.
	*/

	_SIZE_T _offset = 0;
	switch (this->Get_Opcode())
	{
	case DHCP_RES:
	case DHCP_REQ:
		Add_DHCPOption(DHCP_Option(0xff));
		packetLength = 240;
		_offset = packetLength;

		for (auto & option : dhcp_options)
		{
			_offset += Write(&option.second.Option, sizeof(unsigned char), _offset);
			_offset += Write(&option.second.Length, sizeof(unsigned char), _offset);

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

		for (auto & option : tftp_options)
		{
			_offset += Write(option.first.c_str(), static_cast<_SIZE_T>(option.first.size() + 1), _offset);
			_offset += static_cast<_SIZE_T>(Write(&option.second.Value, option.second.Length, _offset) + 1);

			this->set_Length(_offset);
		}

		this->Trim();
		break;
	case TFTP_ERR:
		break;
	case BINL_RSU:
	case BINL_CHA:
		_SIZE_T innerlen = (this->get_Length() - 8);
		Write(&innerlen, 4, 4);
		break;
	}
}

EXPORT INLINE Packet_OPCode Packet::Get_Opcode()
{
	return this->opcode;
}

EXPORT INLINE char* Packet::Get_Buffer()
{
	return this->buffer;
}

EXPORT void Packet::Set_Opcode(Packet_OPCode op)
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
		Write(static_cast<unsigned short>(BS16(3)), 0);
		break;
	case TFTP_RRQ:
		Write(static_cast<unsigned short>(BS16(1)), 0);
		break;
	case TFTP_ACK:
		Write(static_cast<unsigned short>(BS16(4)), 0);
		break;
	case TFTP_ERR:
		Write(static_cast<unsigned short>(BS16(5)), 0);
		break;
	case TFTP_OACK:
		Write(static_cast<unsigned short>(BS16(6)), 0);
		break;
	case BINL_RQU:
		Write(static_cast<unsigned int>(BS32(BINL_RQU)), 0);
		break;
	case BINL_RSU:
		Write(static_cast<unsigned int>(BS32(BINL_RSU)), 0);
		break;
	case BINL_CHA:
		Write(static_cast<unsigned int>(BS32(BINL_CHA)), 0);
		break;
	}
}

EXPORT INLINE void Packet::set_opcode(const DHCP_OPCODE op)
{
	Write(static_cast<unsigned char>(op), 0);
}

EXPORT INLINE DHCP_OPCODE Packet::get_opcode()
{
	unsigned char _tmp = 0;
	Read(&_tmp, 1, 0);

	return static_cast<DHCP_OPCODE>(_tmp);
}

EXPORT INLINE void Packet::set_hwtype(const DHCP_HARDWARETYPE hwtype)
{
	Write(static_cast<unsigned char>(hwtype), 1);
}

EXPORT INLINE DHCP_HARDWARETYPE Packet::get_hwtype()
{
	unsigned char _tmp = 0;
	Read(&_tmp, sizeof(unsigned char), 1);

	return static_cast<DHCP_HARDWARETYPE>(_tmp);
}

EXPORT INLINE void Packet::set_hwlength(const unsigned char length)
{
	Write(static_cast<unsigned char>(length), 2);
}

EXPORT INLINE unsigned char Packet::get_hwlength()
{
	unsigned char _tmp = 0;
	Read(&_tmp, sizeof(unsigned char), 2);

	return _tmp;
}

EXPORT INLINE void Packet::set_hops(const unsigned char hops)
{
	Write(static_cast<unsigned char>(hops), 3);
}

EXPORT INLINE unsigned char Packet::get_hops()
{
	unsigned char _tmp = 0;
	Read(&_tmp, sizeof(unsigned char), 3);

	return _tmp;
}

EXPORT INLINE void Packet::set_xid(const unsigned int xid)
{
	Write(static_cast<unsigned int>(xid), 4);
}

EXPORT INLINE unsigned int Packet::get_xid()
{
	unsigned int _tmp = 0;
	Read(&_tmp, sizeof(unsigned int), 4);

	return _tmp;
}

EXPORT INLINE void Packet::set_secs(const unsigned short secs)
{
	Write(static_cast<unsigned short>(secs), 8);
}

EXPORT INLINE unsigned short Packet::get_secs()
{
	unsigned short _tmp = 0;
	Read(&_tmp, sizeof(unsigned short), 8);

	return _tmp;
}

EXPORT INLINE void Packet::set_flags(const DHCP_FLAGS flags)
{
	Write(static_cast<unsigned short>(flags), 10);
}

EXPORT INLINE DHCP_FLAGS Packet::get_flags()
{
	unsigned short _tmp = 0;
	Read(&_tmp, sizeof(unsigned short), 10);

	return static_cast<DHCP_FLAGS>(_tmp);
}

EXPORT INLINE void Packet::set_clientIP(const _IPADDR ip)
{
	Write(&ip, sizeof(_IPADDR), 12);
}

EXPORT INLINE _IPADDR Packet::get_clientIP()
{
	_IPADDR _tmp = 0;
	Read(&_tmp, sizeof(_IPADDR), 12);

	return _tmp;
}

EXPORT INLINE void Packet::set_yourIP(const _IPADDR ip)
{
	Write(&ip, sizeof(_IPADDR), 16);
}

EXPORT INLINE _IPADDR Packet::get_yourIP()
{
	_IPADDR _tmp = 0;
	Read(&_tmp, sizeof(_IPADDR), 16);

	return _tmp;
}

EXPORT INLINE void Packet::set_nextIP(const _IPADDR ip)
{
	Write(&ip, sizeof(_IPADDR), 20);
}

EXPORT INLINE _IPADDR Packet::get_nextIP()
{
	_IPADDR _tmp = 0;
	Read(&_tmp, sizeof(_IPADDR), 20);

	return _tmp;
}

EXPORT INLINE void Packet::set_relayIP(const _IPADDR ip)
{
	Write(&ip, sizeof(_IPADDR), 24);
}

EXPORT INLINE _IPADDR Packet::get_relayIP()
{
	_IPADDR _tmp = 0;
	Read(&_tmp, sizeof(_IPADDR), 24);

	return _tmp;
}

EXPORT INLINE void Packet::set_hwaddress(const void* mac)
{
	Write(&mac, 16, 28);
}

EXPORT INLINE void Packet::get_hwaddress(char* out, _SIZE_T length)
{
	ClearBuffer(&out, length);

	Read(&out, length, 28);
}

EXPORT INLINE void Packet::set_servername(const std::string& sname)
{
	ClearBuffer(&this->Get_Buffer()[44], 64);
	Write(sname.c_str(), static_cast<_SIZE_T>(sname.size()), 44);
}

EXPORT INLINE std::string Packet::get_servername()
{
	char _tmp[64];
	ClearBuffer(&_tmp, 64);

	Read(&_tmp, 64, 44);

	return std::string(_tmp);
}

EXPORT INLINE void Packet::set_filename(const std::string& file)
{
	ClearBuffer(&this->Get_Buffer()[108], 128);
	Write(file.c_str(), static_cast<_SIZE_T>(file.size()), 108);
}

EXPORT INLINE std::string Packet::get_filename()
{
	char _tmp[128];
	ClearBuffer(&_tmp, 128);

	Read(&_tmp, 128, 108);

	return std::string(_tmp);
}

EXPORT INLINE void Packet::set_cookie()
{
	this->Get_Buffer()[236] = static_cast<unsigned char>(0x63);
	this->Get_Buffer()[237] = static_cast<unsigned char>(0x82);
	this->Get_Buffer()[238] = static_cast<unsigned char>(0x53);
	this->Get_Buffer()[239] = static_cast<unsigned char>(0x63);
}

EXPORT INLINE unsigned int Packet::get_cookie()
{
	unsigned int _tmp = 0;

	Read(&_tmp, sizeof(unsigned int), 236);

	return _tmp;
}

void Packet::set_Length(const _SIZE_T length)
{
	this->packetLength += length;
}

_SIZE_T Packet::get_Length()
{
	return this->packetLength;
}

EXPORT INLINE DHCP_Option Packet::Get_DHCPOption(const unsigned char option)
{
	return this->dhcp_options.at(option);
}

EXPORT INLINE TFTP_Option Packet::Get_TFTPOption(const std::string option)
{
	return this->tftp_options.at(option);
}
