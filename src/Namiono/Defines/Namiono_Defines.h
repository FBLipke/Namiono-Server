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
#include <Environment/environment.h>

typedef enum ServerMode
{
	TCP = 0,
	TCP6 = 1,
	UDP = 2,
	UDP6 = 3,
	UDPMCAST = 4,
	UNKNOWN = 5
} ServerMode;

typedef enum CompareType
{
	Memory = 0,
	String = 1
} CompareType;

typedef enum SocketHandle
{
	READ = 0x00,
	WRITE = 0x01,
	EXCEPT = 0x02
} SocketHandle;

typedef enum Packet_OPCode
{
	DHCP_REQ = 0x00,
	DHCP_RES = 0x01,

	TFTP_RRQ = 0x02,
	TFTP_ACK = 0x03,
	TFTP_ERR = 0x04,
	TFTP_OACK = 0x05,
	TFTP_DAT = 0x06,

	BINL_REQ = 0x07,
	BINL_RES = 0x08,
	BINL_RQU = 0x81525155,
	BINL_NEG = 0x814e4547,
	BINL_CHA = 0x8243484c,
	BINL_RSU = 0x82525355,
	PACKET_UNKNOWN = 0xffffffff

} Packet_OPCode;

typedef enum CLIENTSTATE
{
	DHCP_INIT = 0,
	DHCP_WAITING = 1,
	DHCP_DONE = 2,
	DHCP_ABORT = 3,
	DHCP_RELAY = 4,
	DHCP_SERVERRESPONSE = 5,
	DHCP_CLIENTRESPONSE = 6,
	TFTP_INIT = 7,
	TFTP_DOWNLOAD = 8,
	TFTP_ERROR = 9,
	TFTP_DONE = 10
} CLIENTSTATE;

typedef enum ServiceType
{
	UNKNOWN_SERVICE = 0,
	DHCP_SERVER = 1,
	BINL_SERVER = 2,
	TFTP_SERVER = 3,
	DHCP6_SERVER = 4,
	BINL6_SERVER = 5,
	TFTP6_SERVER = 6,
	HTTP_SERVER = 7
	
} ServiceType;

typedef struct SETTINGS
{
	bool MULTICAST_SUPPORT = false;
	_BYTE DISCOVERY_MODE = 3;
	_BYTE PXEBOOTMENUE = 1;
	_BYTE PXEPROMPTTIMEOUT = 255;
	_BYTE SUBNETDELAY = 2;
	_BYTE PXE_MTFTP_TIMEOUT = 1;
	_BYTE PXE_MTFTP_DELAY = 10;

	_IPADDR UPSTREAMSERVER = 0;

	std::string PXEPROMP = "Press [F8] to boot from network...";
	std::string PXEHDDDESC = "Boot from Harddisk";
	std::string DISCOVERY_ADDR = "224.0.2.1";

	std::string ROOTDIR = "";
	std::string CONFDIR = "";
	std::string NBDOMAIN = "";

	_USHORT TFTP_DEFAULT_BLOCKSIZE = 1024;
	_USHORT MTFTP_SPORT = htons(1759);
	_USHORT MTFTP_CPORT = htons(1758);

	_ULONG SERVER_UPDATE_DELAY = 60;
	_BYTE MAX_HOPS = 4;
	_BYTE WDS_SERVERSELECTION = 1;

	bool SOCKET_BROADCAST = 1;
	bool SOCKET_REUSEADDR = 1;
	bool SOCKET_REUSEPORT = 1;
} SETTINGS;

typedef enum NTLMSSP_FLAGS
{
	NEG_UNICOE = 0x00000001,
	NEG_OEM = 0x00000002,
	REQ_TARGET = 0x00000004,

	NEG_NTLM = 0x00000200,
	NEG_LOCAL = 0x00004000,
	NEG_ASIGN = 0x00008000,

	TAR_DOMAIN = 0x00010000,
	TAR_SERVER = 0x00020000,
	TAR_SHARE = 0x00040000,
	NEG_NTLMV2 = 0x00080000,

	NEG_TARGET = 0x00800000,
	NEG_128BIT = 0x20000000,
	NEG_56BIT = 0x80000000
} NTLMSSP_FLAGS;

typedef enum NTLMSSP_MESSAGETYPE
{
	NTLM_NEGOTIATE = 0x01000000,
	NTLM_CHALLENGE = 0x02000000,
	NTLM_AUTHENTICATE = 0x03000000
} NTLMSSP_MESSAGETYPE;

typedef enum NTLMSSP_TARGETINFO_TYPE
{
	END = 0x0000,
	NBServer = 0x0100,
	NBDomain = 0x0200,
	DNSHost = 0x0300,
	DNSDomain = 0x0400,
	TopDNSDom = 0x0500
} NTLMSSP_TARGETINFO_TYPE;

typedef struct NTLMSSP_TARGETINFO_ENTRY
{
private:
	NTLMSSP_TARGETINFO_TYPE	Type;
	_USHORT	Length;
	const char* Data = nullptr;
public:
	NTLMSSP_TARGETINFO_ENTRY(const NTLMSSP_TARGETINFO_TYPE& type, const char* data, const _SIZET length)
	{
		ClearBuffer(this, sizeof *this);
		this->Length = static_cast<_USHORT>(length);
		this->Type = type;

		this->Data = new char[this->Length];
		ClearBuffer(this, sizeof *this);
		memcpy(&this->Data, data, Length);
	}

	NTLMSSP_TARGETINFO_ENTRY()
	{
		this->Type = NTLMSSP_TARGETINFO_TYPE::END;
		this->Length = 0;

		delete[] this->Data;
		this->Data = nullptr;
	}

} NTLMSSP_TARGETINFO_ENTRY;

typedef struct NTLMSSP_SECBUFFER
{
private:
	_USHORT	Length;
	_USHORT	AllocatedSpace;
	_SIZET	Position;
public:
	NTLMSSP_SECBUFFER()
	{
		ClearBuffer(this, sizeof *this);
	};

	~NTLMSSP_SECBUFFER()
	{
		ClearBuffer(this, sizeof *this);
	};

	void Set_Length(const _SIZET& length)
	{
		this->Length = static_cast<_USHORT>(length);
		this->AllocatedSpace = this->Length;
	}

	_USHORT Get_Length()
	{
		return this->Length;
	}

	void Set_Position(const _SIZET& position)
	{
		this->Position = position;
	}
} NTLMSSP_SECBUFFER; /* 8 Byte */

typedef struct NTLMSSP_MESSAGE
{
private:
	char				header[9];
	NTLMSSP_MESSAGETYPE	msgtype;
	NTLMSSP_SECBUFFER	targetName;
	NTLMSSP_FLAGS		flags;
	char				challenge[8]; /* 24 */
	char				context[8];	/* 32 */
	NTLMSSP_SECBUFFER	targetInfo; /* 40 */
	char				targetNameData[64]; /* 48 */

	_SIZET AS_UTF16LE(const char* src, char* dest, size_t offset)
	{
		_SIZET ulen = 0, i = 0;

		for (i = 0; i < static_cast<_SIZET>(strlen(src)); i++)
		{
			dest[offset + ulen] = src[i];
			ulen += 2;
		}

		return ulen;
	}
public:
	std::vector<std::unique_ptr<NTLMSSP_TARGETINFO_ENTRY>> targetInfoData; /* 60 */

	NTLMSSP_MESSAGE()
	{
		ClearBuffer(this, sizeof *this);
		sprintf(header, "NTLMSSP");
	}

	~NTLMSSP_MESSAGE()
	{
		targetInfoData.clear();
		ClearBuffer(this, sizeof *this);
	}

	void Set_MessageType(const NTLMSSP_MESSAGETYPE& msgtype)
	{
		this->msgtype = msgtype;
	}

	void Set_Challenge(const char* challenge)
	{
		memcpy(this->challenge, challenge, 8);
	}

	void Set_Context(const char* ctx)
	{
		memcpy(this->context, ctx, 8);
	}

	void Set_Flags(const NTLMSSP_FLAGS& flags)
	{
		this->flags = flags;
	}

	void Set_TargetName(const std::string& name)
	{
		this->targetName.Set_Length(AS_UTF16LE(
			name.c_str(), this->targetNameData, 0));

		this->targetName.Set_Position(48);
		this->targetInfoData.emplace_back(std::make_unique<NTLMSSP_TARGETINFO_ENTRY>
			(NBDomain, this->targetNameData, this->targetInfo.Get_Length()));
	}
} NTLMSSP_MESSAGE;

