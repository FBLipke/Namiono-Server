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

typedef enum DHCP_RELAYAGENTOptions
{
	CircuitID = 1,
	RemoteID = 2
} DHCP_RELAYAGENTOptions;

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


typedef struct LeaseInfo
{
	std::string mac;
	std::string addr;
} LeaseInfo;

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

typedef enum TFTP_OPCODE
{
	TFTP_Read = 0x0001,
	TFTP_Write = 0x0002,
	TFTP_Data = 0x0003,
	TFTP_Ack = 0x0004,
	TFTP_Error = 0x0005,
	TFTP_Opt = 0x0006
} TFTP_OPCODE;

typedef enum PXELinux_Options
{
	PXELINUX_MAGIC = 208,
	PXELINUX_CONFIGFILE = 209,
	PXELINUX_PATHPREFIX = 210,
	PXELINUX_REBOOTTIME = 211
} PXELinux_Options;

typedef enum IPXE_Options
{
	IPXE_PRIORITY = 1, // Byte
	IPXE_KEEP_SAN = 8, // Byte
	IPXE_SKIP_SAN_BOOT = 9, // Byte
	IPXE_PXEEXT = 16, // Byte
	IPXE_ISCSI = 17, // Byte
	IPXE_AOE = 18, // Byte
	IPXE_HTTP = 19, // Byte
	IPXE_HTTPS = 20, // Byte
	IPXE_TFTP = 21, // Byte
	IPXE_FTP = 22, // Byte
	IPXE_DNS = 23, // Byte
	IPXE_BZIMAGE = 24, // Byte
	IPXE_MULTIBOOT = 25, // Byte
	IPXE_SLAM = 26, // Byte
	IPXE_SRP = 27, // Byte
	IPXE_NBI = 32, // Byte
	IPXE_PXE = 33, // Byte
	IPXE_ELF = 34, // Byte
	IPXE_COMBOOT = 35, // Byte
	IPXE_EFI = 36, // Byte
	IPXE_FCOE = 37, // Byte
	IPXE_VLAN = 38, // Byte
	IPXE_MENU = 39, // Byte
	IPXE_SDI = 40, // Byte
	IPXE_NFS = 41, // Byte
	IPXE_SYSLOGS = 85, // String
	IPXE_CERT = 91, // String
	IPXE_privkey = 92, // String
	IPXE_CROSSCERT = 93, // String
	IPXE_NO_PROXYDHCP = 176, // Byte
	IPXE_BUSID = 177, // String
	IPXE_BIOSDRIVE = 189, // Byte
	IPXE_USERNAME = 190, // String
	IPXE_PASSWORD = 191, // String
	IPXE_RUSERNAME = 192, // String
	IPXE_RPASSWORD = 193, // String
	IPXE_VERSION = 235, // String
	IPXE_ISCSIINITIQN = 203 // String
} IPXE_Options;

typedef struct TFTP_Option
{
	std::string Option = "";
	_SIZET Length = 0;
	char Value[256];

	TFTP_Option()
	{
		ClearBuffer(this, sizeof(*this));
	}

	TFTP_Option(const std::string& opt, const std::string& value)
	{
		ClearBuffer(Value, (value.size() + 1));

		Option = opt;
		Length = static_cast<_SIZET>(value.size());
		memcpy(&Value, value.c_str(), value.size());
	}

	TFTP_Option(const _BYTE& opt)
	{
		ClearBuffer(Value, 1);

		Option = opt;
		Length = 0;
	}

	TFTP_Option(const std::string& opt, _SIZET& length, const _USHORT& value)
	{
		ClearBuffer(Value, 2);
		Option = opt;
		memcpy(&Value, &value, 2);
	}

	~TFTP_Option()
	{

	}
} TFTP_Option;

typedef struct DHCP_Option
{
	DHCP_Option()
	{
		ClearBuffer(Value, Length);
	}

	~DHCP_Option()
	{

	}

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

		ClearBuffer(Value, Length + 1);

		if (Length != 0)
			memcpy(&Value, &value, Length);
	}

	DHCP_Option(const _BYTE& opt, const _USHORT& value)
	{
		Option = opt;
		Length = 2;

		ClearBuffer(&Value, Length + 1);

		if (Length != 0)
			memcpy(&Value, &value, Length);
	}

	DHCP_Option(const _BYTE& opt, const _ULONG& value)
	{
		Option = opt;
		Length = 4;

		ClearBuffer(&Value, Length + 1);

		if (Length != 0)
			memcpy(&Value, &value, Length);
	}

	DHCP_Option(const _BYTE& opt, const _UINT& value)
	{
		Option = opt;
		Length = 4;

		ClearBuffer(&Value, Length + 1);

		if (Length != 0)
			memcpy(&Value, &value, Length);
	}

	DHCP_Option(const _BYTE& opt, const std::vector<DHCP_Option>& value)
	{
		Option = opt;
		Length = 0;
		int offset = 0;

		// Get the entire option length!

		for (_SIZET i = 0; i < value.size(); i++)
			Length += value.at(i).Length + 2;

		if (Length > 255)
			return;

		ClearBuffer(&Value, Length + 1);

		for (_SIZET i = 0; i < value.size(); i++)
		{
			memcpy(&Value[offset], &value.at(i).Option, 1);
			offset += 1;

			memcpy(&Value[offset], &value.at(i).Length, 1);
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
		memcpy(&val, &this->Value, sizeof val);

		return static_cast<_USHORT>(val);
	}

	_IPADDR Get_Value_As_IPADDR()
	{
		_IPADDR val = 0;
		memcpy(&val, &this->Value, sizeof val);

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
		const BootServerType type = PXEBootstrapServer)
	{
		Ident = id;
		Type = type;
		Description = text;
		DescLength = static_cast<_BYTE>(Description.size());
	}

	BootMenuEntry(const _USHORT& id, const std::string& text, const _IPADDR& address,
		const BootServerType type = PXEBootstrapServer)
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

typedef enum DHCP_OPCODE
{
	BOOTREQUEST = 0x01,
	BOOTREPLY = 0x02,

	BINL_REQUEST = 0x81,
	BINL_REPLY = 0x82
} DHCP_OPCODE;

typedef enum RBCP_LAYER
{
	Bootfile = 0x0000,
	Credentials = 0x0001

} RBCP_LAYER;

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

typedef enum DHCP_ARCH
{
	INTEL_X86 = 0x0000,
	NEC_PC98 = 0x0001,
	EFI_ITAN = 0x0002,
	DEC_ALPHA = 0x0003,
	ARC_X86 = 0x0004,
	INTEL_LEAN = 0x0005,
	INTEL_IA32X64 = 0x0006,
	EFI_BC = 0x0007,
	EFI_XSCALE = 0x0008,
	EFI_X86X64 = 0x0009
} DHCP_ARCH;

typedef enum DHCP_FLAGS
{
	Unicast = 0,
	Broadcast = 128
} DHCP_FLAGS;

typedef enum DHCP_HARDWARETYPE
{
	Ethernet = 0x01,
	Exp_Ethernet = 0x02,
	AX25 = 0x03,
	PRONET = 0x04,
	CHAOS = 0x05,
	IEEE802 = 0x06,
	ARCNET = 0x07
} DHCP_HARDWARETYPE;

typedef enum CLIENTSTATE
{
	DHCP_INIT = 0,
	DHCP_WAITING = 1,
	DHCP_DONE = 2,
	DHCP_ABORT = 3,
	DHCP_RELAY = 4,
	TFTP_INIT = 5,
	TFTP_DOWNLOAD = 6,
	TFTP_ERROR = 7,
	TFTP_DONE = 8
} CLIENTSTATE;

typedef enum WDSNBP_ActionDone_Values
{
	False = 0,
	True = 1
} WDSNBP_NEXTACTION_Values;

typedef enum WDSNBP_Options
{
	WDSBP_OPT_ARCHITECTURE = 1,
	WDSBP_OPT_NEXT_ACTION = 2,
	WDSBP_OPT_POLL_INTERVAL = 3,
	WDSBP_OPT_POLL_RETRY_COUNT = 4,
	WDSBP_OPT_REQUEST_ID = 5,
	WDSBP_OPT_MESSAGE = 6,
	WDSBP_OPT_VERSION_QUERY = 7,
	WDSBP_OPT_SERVER_VERSION = 8,
	WDSBP_OPT_REFERRAL_SERVER = 9,
	WDSBP_OPT_PXE_CLIENT_PROMPT = 11,
	WDSBP_OPT_PXE_PROMPT_DONE = 12,
	WDSBP_OPT_NBP_VER = 13,
	WDSBP_OPT_ACTION_DONE = 14,
	WDSBP_OPT_ALLOW_SERVER_SELECTION = 15,
	WDSBP_OPT_SERVER_FEATURES = 16,
	WDSBP_OPT_END = 0xff
} WDSNBP_Options;

typedef enum BSDP_Options
{
	BSDP_MessageType = 0x01,
	BSDP_Version = 0x02,
	BSDP_Address = 0x03,
	BSDP_Priority = 0x04,
	BSDP_RPort = 0x05,
	BSDP_Path = 0x06,
	BSDP_ImageID = 0x07,
	BSDP_SelectedImageID = 0x08,
	BSDP_ImageList = 0x09,
	BSDP_LEGACY = 0x10, // Handle as 255
	BSDP_Attributes = 0x11,
	BSDP_ShadowPath = 0x80,
	BSDP_ShadowFile = 0x81,
	BSDP_MACHINENAME = 0x82

} BSDP_Options;

typedef enum BSDP_ATTIBUTE
{
	None = 0x0000,
	Install = 0x8100,
	NoInstall = 0x0100,
	Diagnostic = 0x0300
} BSDP_ATTIBUTE;


typedef enum BSDP_MSGTYPE
{
	LIST = 0x01,
	SELECT = 0x02,
	FAILED = 0x03
} BSDP_MSGTYPE;

typedef enum BSDP_VERSION
{
	VERSION_1_0 = 0x0100,
	VERSION_1_1 = 0x0101
} BSDP_VERSION;

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

typedef enum WDSNBP_OPTION_NEXTACTION
{
	APPROVAL = 0x01,
	REFERRAL = 0x03,
	ABORT = 0x05
} WDSNBP_OPTION_NEXTACTION;

typedef enum ServiceType
{
	DHCP_SERVER = 0,
	BINL_SERVER = 1,
	TFTP_SERVER = 3,
	DHCP6_SERVER = 4,
	BINL6_SERVER = 5,
	TFTP6_SERVER = 6
} ServiceType;

typedef struct IPXE
{
	IPXE()
	{
		this->username = new std::string("");
		this->password = new std::string("");
	}

	~IPXE()
	{
		delete this->username;
		this->username = nullptr;

		delete this->password;
		this->password = nullptr;
	}

	void Set_Username(const std::string& username)
	{
		*this->username = username;
	}

	void Set_Password(const std::string& password)
	{
		*this->password = password;
	}

	const std::string& Get_Username() const
	{
		return *this->username;
	}

	const std::string& Get_Password() const
	{
		return *this->password;
	}
private:
	std::string* username = nullptr;
	std::string* password = nullptr;
} IPXE;

typedef struct BSDP
{
	BSDP()
	{
		this->MSGTYPE = new BSDP_MSGTYPE(LIST);
		this->Priority = new _USHORT(0x0a);
		this->BootImage = new _UINT(1);
		this->SelectedImage = new _UINT(1);
		this->attributes = new BSDP_ATTIBUTE(Install);
		this->ServerIdent = new _IPADDR(0);
		this->ReplyPort = new _USHORT(0);
	}

	~BSDP()
	{
		delete this->attributes;
		this->attributes = nullptr;

		delete this->BootImage;
		this->BootImage = nullptr;

		delete this->Priority;
		this->Priority = nullptr;

		delete this->MSGTYPE;
		this->MSGTYPE = nullptr;

		delete this->ServerIdent;
		this->ServerIdent = nullptr;

		delete this->ReplyPort;
		this->ReplyPort = nullptr;
	}

	void Set_ServerIdent(const _IPADDR& ident)
	{
		*this->ServerIdent = ident;
	}

	void Set_BootImage(const _UINT& id)
	{
		*this->BootImage = id;
	}

	_UINT Get_BootImage() const
	{
		return *this->BootImage;
	}

	void Set_SelectedImage(const _UINT& id)
	{
		*this->SelectedImage = id;
	}

	_UINT Get_SelectedImage() const
	{
		return *this->SelectedImage;
	}

	bool Get_ServerIdent() const
	{
		return *this->ServerIdent;
	}

	void Set_ReplyPort(const _USHORT& port)
	{
		*this->ReplyPort = port;
	}

	_USHORT Get_ReplyPort() const
	{
		return *this->ReplyPort;
	}

	void Set_Attributes(const BSDP_ATTIBUTE& attribs)
	{
		*this->attributes = attribs;
	}

	BSDP_ATTIBUTE Get_Attributes() const
	{
		return *this->attributes;
	}

private:
	BSDP_MSGTYPE* MSGTYPE = nullptr;
	BSDP_VERSION* Version = nullptr;
	BSDP_ATTIBUTE* attributes = nullptr;
	_IPADDR* ServerIdent = nullptr;
	_USHORT* Priority = nullptr;
	_UINT* BootImage = nullptr;
	_UINT* SelectedImage = nullptr;
	_USHORT* ReplyPort = nullptr;

} BSDP;

typedef struct RBCP
{
	RBCP()
	{
		this->item = new _USHORT(0);
		this->layer = new RBCP_LAYER(RBCP_LAYER::Bootfile);

		this->mcastip = new _IPADDR(0);
		this->control = new RBCP_DISCOVERYCONTROL(DISABLE_MCAST);
	};

	~RBCP()
	{
		delete this->item;
		this->item = nullptr;

		delete this->layer;
		this->layer = nullptr;

		delete this->control;
		this->control = nullptr;

		delete this->mcastip;
		this->mcastip = nullptr;
	};

	void Set_Item(const _USHORT& item)
	{
		*this->item = item;
	}

	const _USHORT& Get_Item() const
	{
		return *this->item;
	}

	void Set_Layer(const RBCP_LAYER& layer)
	{
		*this->layer = layer;
	}

	const RBCP_LAYER& Get_Layer() const
	{
		return *this->layer;
	}

	void Set_MulticastIP(const _IPADDR& ip)
	{
		*this->mcastip = ip;
	}

	const _IPADDR& Get_MulticastIP() const
	{
		return *this->mcastip;
	}



private:
	_USHORT* item = nullptr;
	RBCP_LAYER* layer = nullptr;

	_IPADDR* mcastip = nullptr;
	RBCP_DISCOVERYCONTROL* control = nullptr;
} RBCP;

static struct SETTINGS
{
	bool MULTICAST_SUPPORT = false;
	_BYTE DISCOVERY_MODE = RBCP_DISCOVERYCONTROL::UNICAST_ONLY;
	_BYTE PXEBOOTMENUE = 1;
	_BYTE PXEPROMPTTIMEOUT = 255;
	_BYTE SUBNETDELAY = 2;
	_BYTE PXE_MTFTP_TIMEOUT = 1;
	_BYTE PXE_MTFTP_DELAY = 10;

	std::string PXEPROMP = "Press [F8] to boot from network...";
	std::string PXEHDDDESC = "Boot from Harddisk";
	std::string DISCOVERY_ADDR = "224.0.2.1";

	std::string NBDOMAIN = "FBLIPKE";

	_USHORT TFTP_DEFAULT_BLOCKSIZE = 1024;
	_USHORT MTFTP_SPORT = 1759;
	_USHORT MTFTP_CPORT = 1758;

	_ULONG SERVER_UPDATE_DELAY = 60;
	_BYTE MAX_HOPS = 4;

	bool SOCKET_BROADCAST = 1;
	bool SOCKET_REUSEADDR = 1;
	bool SOCKET_REUSEPORT = 1;

} SETTINGS;

typedef struct WDS
{
	WDS()
	{
		this->NextAction = new _BYTE(WDSNBP_OPTION_NEXTACTION::APPROVAL);
		this->ActionDone = new WDSNBP_ActionDone_Values(True);
		this->PollIntervall = new _USHORT(SETTINGS.PXE_MTFTP_DELAY);
		this->RetryCount = new _USHORT(65535);
		this->requestid = new _ULONG(1);
		this->bcdfile = new std::string("");
		this->referalIP = new _IPADDR(0);
		this->AdminMessage = new std::string("Namiono-Server 0.5");
		this->ServerSelection = new bool(0);
	}

	~WDS()
	{
		delete this->NextAction;
		this->NextAction = nullptr;

		delete this->ActionDone;
		this->ActionDone = nullptr;

		delete this->PollIntervall;
		this->PollIntervall = nullptr;

		delete this->RetryCount;
		this->RetryCount = nullptr;

		delete this->requestid;
		this->requestid = nullptr;

		delete this->bcdfile;
		this->bcdfile = nullptr;

		delete this->referalIP;
		this->referalIP = nullptr;

		delete this->AdminMessage;
		this->AdminMessage = nullptr;

		delete this->ServerSelection;
		this->ServerSelection = nullptr;
	}

	std::string& GetBCDfile() const
	{
		return *this->bcdfile;
	}

	void SetReferralServer(const _IPADDR& addr)
	{
		*this->referalIP = addr;
	}

	const _IPADDR& GetReferalServer() const
	{
		return *this->referalIP;
	}

	void SetRequestID(const _ULONG& id)
	{
		*this->requestid = id;
	}

	const _ULONG& GetRequestID() const
	{
		return *this->requestid;
	}

	void SetActionDone(const WDSNBP_ActionDone_Values& done)
	{
		*this->ActionDone = done;
	}

	const WDSNBP_ActionDone_Values& GetActionDone() const
	{
		return *this->ActionDone;
	}

	void SetWDSMessage(const std::string& message)
	{
		*this->AdminMessage = message;
	}

	const std::string& GetWDSMessage() const
	{
		return *this->AdminMessage;
	}

	void SetNextAction(const WDSNBP_OPTION_NEXTACTION& action)
	{
		*this->NextAction = action;
	}

	const _BYTE& GetNextAction() const
	{
		return *this->NextAction;
	}

	void SetRetryCount(const _USHORT& action)
	{
		*this->RetryCount = action;
	}

	const _USHORT& GetRetryCount() const
	{
		return *this->RetryCount;
	}

	void SetPollInterval(const _USHORT& interval)
	{
		*this->PollIntervall = interval;
	}

	const _USHORT& GetPollInterval() const
	{
		return *this->PollIntervall;
	}

	void SetBCDfile(const std::string& file)
	{
		*this->bcdfile = file;
	}
private:
	_BYTE* NextAction = nullptr;
	WDSNBP_ActionDone_Values* ActionDone = nullptr;
	_USHORT* PollIntervall = nullptr;
	_USHORT* RetryCount = nullptr;
	_ULONG* requestid = nullptr;
	std::string* bcdfile = nullptr;
	_IPADDR* referalIP = nullptr;
	std::string* AdminMessage = nullptr;
	bool* ServerSelection = nullptr;
} WDS;

typedef struct DHCP_CLIENT
{
	DHCP_CLIENT(const std::string& nextIP)
	{
		this->wds = new WDS();
		this->rbcp = new RBCP();
		this->bsdp = new BSDP();
		this->ipxe = new IPXE();
		this->arch = new DHCP_ARCH(INTEL_X86);
		this->bootfile = new std::string("");
		this->prefix = new std::string("");
		this->vendorid = new DHCP_VENDOR(UNKNOWNNO);
		this->vendorstring = new std::string("");
		this->msgtype = new DHCP_MSGTYPE(OFFER);
		this->NextServer = new _IPADDR(inet_addr(nextIP.c_str()));
		this->isWDSRequest = new bool(false);
		this->isIPXERequest = new bool(false);
		this->isBSDPRequest = new bool(true);
		this->isRelayedPacket = new bool(false);
		this->state = new CLIENTSTATE(DHCP_INIT);
		this->vendorOpts = new std::vector<DHCP_Option>();
	}

	~DHCP_CLIENT()
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

	void SetNextServer(const _IPADDR& ip)
	{
		*this->NextServer = ip;
	}

	const _IPADDR& GetNextServer() const
	{
		return *this->NextServer;
	}

	void Set_State(const CLIENTSTATE& state)
	{
		*this->state = state;
	}

	const CLIENTSTATE& Get_State() const
	{
		return *this->state;
	}

	void Set_Vendor(const DHCP_VENDOR& vendor)
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

	const DHCP_VENDOR& Get_Vendor() const
	{
		return *this->vendorid;
	}

	void Set_VendorString(const std::string& vendor)
	{
		*this->vendorstring = vendor;
	}

	const std::string& Get_VendorString() const
	{
		return *this->vendorstring;
	}

	void SetMessageType(const DHCP_MSGTYPE& type)
	{
		*this->msgtype = type;
	}

	const DHCP_MSGTYPE& GetMessageType() const
	{
		return *this->msgtype;
	}

	void SetBootfile(const std::string& file)
	{
		*this->bootfile = file;
	}

	const std::string& GetPrefix() const
	{
		return *this->bootfile;
	}

	void SetPrefix(const std::string& path)
	{
		*this->prefix = path;
	}

	const DHCP_ARCH& GetArchitecture() const
	{
		return *this->arch;
	}

	void SetArchitecture(const DHCP_ARCH& arch)
	{
		*this->arch = static_cast<DHCP_ARCH>(BS16(arch));
	}

	const bool GetIsWDSRequest() const
	{
		return *this->isWDSRequest;
	}

	const bool GetIsIPXERequest() const
	{
		return *this->isIPXERequest;
	}

	void SetIsIPXERequest(bool is)
	{
		*this->isIPXERequest = is;
	}

	void SetIsWDSRequest(bool is)
	{
		*this->isWDSRequest = is;
	}

	void SetIsBSDPRequest(bool is)
	{
		*this->isBSDPRequest = is;
	}

	const bool GetIsBSDPRequest() const
	{
		return *this->isBSDPRequest;
	}

	const bool GetIsRelayedPacket() const
	{
		return *this->isRelayedPacket;
	}

	void SetIsRelayedPacket(bool is)
	{
		*this->isRelayedPacket = is;
	}

	const std::string GetBootfile()
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
					//*this->bootfile = *this->prefix + "wdsnbp.com";
					*this->bootfile = "Boot\\bstrap.0";
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

	WDS* wds = nullptr;
	RBCP* rbcp = nullptr;
	BSDP* bsdp = nullptr;
	IPXE* ipxe = nullptr;
	std::vector<DHCP_Option>* vendorOpts = nullptr;

private:
	bool* isWDSRequest = nullptr;
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

} DHCP_CLIENT;

typedef struct TFTP_CLIENT
{
	void Set_State(const CLIENTSTATE& state)
	{
		*this->tftp_state = state;
	}

	const CLIENTSTATE& Get_State() const
	{
		return *this->tftp_state;
	}

	bool OpenFile(const std::string& path)
	{
		_SIZET bytes = 0;

		this->filehandle = fopen(path.c_str(), "rb");

		if (fseek(this->filehandle, 0, SEEK_END) == 0)
		{
			bytes = static_cast<_SIZET>(ftell(this->filehandle));
			rewind(this->filehandle);

			this->SetBytesToRead(bytes);
		}

		return this->GetBytesToRead() != 0;
	}

	void FileSeek(const long& pos)
	{
		fseek(this->filehandle, pos, SEEK_SET);
	}

	void FileSeek()
	{
		fseek(this->filehandle, static_cast<long>(this->GetBytesRead()), SEEK_SET);
	}

	void CloseFile()
	{
		if (this->filehandle != nullptr)
		{
			fclose(this->filehandle);
			this->filehandle = nullptr;
		}
	}

	FILE* Get_FileHandle()
	{
		return this->filehandle;
	}

	void SetCurrentBlock()
	{
		*this->block += 1;
	}

	void SetCurrentBlock(const _USHORT& block)
	{
		*this->block = block;
	}

	const _BYTE& GetWindowSize() const
	{
		return *this->windowsize;
	}

	void SetWindowSize(const _BYTE& window)
	{
		*this->windowsize = window;
	}

	const _USHORT& GetBlockSize() const
	{
		return *this->blocksize;
	}

	void SetBlockSize(const _USHORT& blocksize)
	{
		*this->blocksize = blocksize;
	}

	const _USHORT& GetMSFTWindow() const
	{
		return *this->msftwindow;
	}

	void SetMSFTWindow(const _USHORT& window)
	{
		*this->msftwindow = window;
	}

	const _USHORT& GetCurrentBlock() const
	{
		return *this->block;
	}

	const _SIZET& GetBytesToRead() const
	{
		return *this->bytesToRead;
	}

	void SetBytesToRead(const _SIZET& bytes)
	{
		*this->bytesToRead = bytes;
	}

	const _SIZET& GetBytesRead() const
	{
		return *this->bytesread;
	}

	void SetBytesRead(const _SIZET& bytes)
	{
		if (bytes == 0)
			*this->bytesread = bytes;
		else
			*this->bytesread += bytes;
	}

	const std::string& GetFilename() const
	{
		return *this->filename;
	}

	void SetFilename(const std::string& filename)
	{
		*this->filename = filename;
	}

	TFTP_CLIENT()
	{
		this->filename = new std::string("");
		this->block = new _USHORT(0);
		this->blocksize = new _USHORT(SETTINGS.TFTP_DEFAULT_BLOCKSIZE);
		this->bytesread = new _SIZET(0);
		this->bytesToRead = new _SIZET(0);
		this->msftwindow = new _USHORT(27182);
		this->retries = new _BYTE(0);
		this->tftp_state = new CLIENTSTATE(CLIENTSTATE::TFTP_INIT);
		this->windowsize = new _BYTE(1);
		this->filehandle = nullptr;
	}

	~TFTP_CLIENT()
	{
		delete this->filename;
		this->filename = nullptr;

		delete this->block;
		this->block = nullptr;

		delete this->blocksize;
		this->blocksize = nullptr;

		delete this->bytesread;
		this->bytesread = nullptr;

		delete this->bytesToRead;
		this->bytesToRead = nullptr;

		delete this->msftwindow;
		this->msftwindow = nullptr;

		delete this->retries;
		this->retries = nullptr;

		delete this->tftp_state;
		this->tftp_state = nullptr;

		delete this->windowsize;
		this->windowsize = nullptr;

		this->CloseFile();
	}

private:
	_BYTE* retries = nullptr;
	_BYTE* windowsize = nullptr;
	_USHORT* msftwindow = nullptr;
	_USHORT* block = nullptr;
	_USHORT* blocksize = nullptr;
	CLIENTSTATE* tftp_state = nullptr;
	std::string* filename = nullptr;

	_SIZET* bytesread = nullptr;
	_SIZET* bytesToRead = nullptr;
	FILE* filehandle = nullptr;
} TFTP_CLIENT;

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
		this->targetInfoData.emplace_back(new NTLMSSP_TARGETINFO_ENTRY(
			NBDomain, this->targetNameData, this->targetInfo.Get_Length()));
	}
} NTLMSSP_MESSAGE;

