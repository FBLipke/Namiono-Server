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

typedef enum RBCP_DISCOVERYCONTROL
{
	DISABLE_BCAST = 1,
	DISABLE_MCAST = 2,
	UNICAST_ONLY = DISABLE_BCAST | DISABLE_MCAST,
	SERVERLIST_ONLY = 4,
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
	BINL_RSU = 0x82525355

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

	TFTP_Option(const _BYTE opt)
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
		Option = opt;
		Length = length;

		ClearBuffer(&Value, Length + (_BYTE)1);

		if (length != 0)
			memcpy(&Value, value, Length);
	}

	DHCP_Option(const _BYTE& opt, const std::string& value)
	{
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

	DHCP_Option(const _BYTE& opt, const _UINT& value)
	{
		Option = opt;
		Length = 4;

		ClearBuffer(&Value, Length + 1);

		if (Length != 0)
			memcpy(&Value, &value, Length);
	}

	DHCP_Option(const _BYTE& opt, const _IPADDR& value)
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

		// Get the entire options length!
		for (const auto & option : value)
			Length += option.Length + 2;

		ClearBuffer(&Value, Length + 1);

		for (const auto & option : value)
		{
			memcpy(&Value[offset], &option.Option, 1);
			offset += 1;

			memcpy(&Value[offset], &option.Length, 1);
			offset += 1;

			memcpy(&Value[offset], &option.Value, option.Length);
			offset += option.Length;
		}

		Length = offset;
	}

	DHCP_Option(const _BYTE& opt)
	{
		Option = opt;
		Length = 0;
		ClearBuffer(&Value, Length + 1);
	}

	std::string Get_Value_As_String()
	{
		
		return std::string(this->Value);
	}

	_BYTE Get_Value_As_Byte()
	{
		return static_cast<_BYTE>(this->Value[0]);
	}

	_USHORT Get_Value_As_USHORT()
	{
		_USHORT val = 0;
		memcpy(&val, &this->Value, sizeof val);

		return static_cast<_USHORT>(val);
	}

	_ULONG Get_Value_As_IPADDR()
	{
		_ULONG val = 0;
		memcpy(&val, &this->Value, sizeof val);

		return static_cast<_ULONG>(val);
	}

	void Get_SubOptions(std::vector<DHCP_Option>& suboptionList)
	{
		for (_SIZET i = 0; i < Length; i++)
		{
			if (static_cast<_BYTE>(Value[i]) == static_cast<_BYTE>(0xff) ||
				static_cast<_BYTE>(Value[i]) == 0x00)
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

	BootServerEntry(const _USHORT id, const std::string& desc, std::vector<_IPADDR>& adresses,
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

	BootMenuEntry(const _USHORT id, const std::string& text,
		const BootServerType type = PXEBootstrapServer)
	{
		Ident = id;
		Type = type;
		Description = text;
		DescLength = static_cast<_BYTE>(Description.size());
	}

	BootMenuEntry(const _USHORT id, const std::string& text, const _IPADDR address,
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
		ClearBuffer(this, sizeof(*this));
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
	APPLEBSDP = 0x03
} DHCP_VENDOR;

typedef enum DHCP_ARCH
{
	INTEL_X86 = 0x00,
	NEC_PC98 = 0x01,
	EFI_ITAN = 0x02,
	DEC_ALPHA = 0x03,
	ARC_X86 = 0x04,
	INTEL_LEAN = 0x05,
	INTEL_IA32X64 = 0x06,
	EFI_BC = 0x07,
	EFI_XSCALE = 0x08,
	EFI_X86X64 = 0x09
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
	TFTP_INIT = 4,
	TFTP_DOWNLOAD = 5,
	TFTP_ERROR = 6,
	TFTP_DONE = 7
} CLIENTSTATE;

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
	BSDP_Attributes = 0x11

}BSDP_Options;

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
		delete this->password;
	}

	void Set_Username(const std::string& username)
	{
		*this->username = username;
	}

	void Set_Password(const std::string& password)
	{
		*this->password = password;
	}

	std::string Get_Username()
	{
		return *this->username;
	}

	std::string Get_Password()
	{
		return *this->password;
	}
private:
	std::string* username;
	std::string* password;
} IPXE;

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
		delete this->layer;
		delete this->control;
		delete this->mcastip;
	};

	void Set_Item(const _USHORT& item)
	{
		*this->item = item;
	}

	_USHORT Get_Item()
	{
		return *this->item;
	}

	void Set_Layer(const RBCP_LAYER& layer)
	{
		*this->layer = layer;
	}

	RBCP_LAYER Get_Layer()
	{
		return *this->layer;
	}

	void Set_MulticastIP(const _IPADDR& ip)
	{
		*this->mcastip = ip;
	}

	_IPADDR Get_MulticastIP()
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
	_BYTE DISCOVERY_MODE = UNICAST_ONLY;
	_BYTE PXEBOOTMENUE = 1;
	_BYTE PXEPROMPTTIMEOUT = 255;
	_BYTE SUBNETDELAY = 1;
	_BYTE PXE_MTFTP_TIMEOUT = 1;
	_BYTE PXE_MTFTP_DELAY = 10;

	std::string PXEPROMP = "Press [F8] to boot from network...";
	std::string DISCOVERY_ADDR = "224.0.1.2";
	
	std::string NBDOMAIN = "FBLIPKE";

	_USHORT TFTP_DEFAULT_BLOCKSIZE = 1024;
	_USHORT MTFTP_SPORT = 1759;
	_USHORT MTFTP_CPORT = 1758;

	_ULONG SERVER_UPDATE_DELAY = 60;
	_BYTE MAX_HOPS = 4;
} SETTINGS;

typedef struct WDS
{
	WDS()
	{
		this->NextAction = new _BYTE(WDSNBP_OPTION_NEXTACTION::APPROVAL);
		this->ActionDone = new _BYTE(1);
		this->PollIntervall = new _USHORT(SETTINGS.PXE_MTFTP_DELAY);
		this->RetryCount = new _USHORT(65535);
		this->requestid = new _ULONG(1);
		this->bcdfile = new std::string("");
		this->referalIP = new _IPADDR(0);
		this->AdminMessage = new std::string("");
		this->ServerSelection = new bool(0);
	}

	~WDS()
	{
		delete this->NextAction;
		delete this->ActionDone;
		delete this->PollIntervall;
		delete this->RetryCount;
		delete this->requestid;
		delete this->bcdfile;
		delete this->referalIP;
		delete this->AdminMessage;
		delete this->ServerSelection;
	}

	std::string GetBCDfile()
	{
		return *this->bcdfile;
	}

	void SetReferralServer(_IPADDR addr)
	{
		*this->referalIP = addr;
	}

	_IPADDR GetReferalServer()
	{
		return *this->referalIP;
	}

	void SetRequestID(_ULONG id)
	{
		*this->requestid = id;
	}

	_ULONG GetRequestID()
	{
		return *this->requestid;
	}

	void SetActionDone(const _BYTE& done)
	{
		*this->ActionDone = done;
	}

	_BYTE& GetActionDone()
	{
		return *this->ActionDone;
	}

	void SetWDSMessage(std::string message)
	{
		*this->AdminMessage = message;
	}

	std::string GetWDSMessage()
	{
		return *this->AdminMessage;
	}

	void SetNextAction(WDSNBP_OPTION_NEXTACTION action)
	{
		*this->NextAction = action;
	}

	_BYTE GetNextAction()
	{
		return *this->NextAction;
	}

	void SetRetryCount(_USHORT action)
	{
		*this->RetryCount = action;
	}

	_USHORT GetRetryCount()
	{
		return *this->RetryCount;
	}

	void SetPollInterval(_USHORT interval)
	{
		*this->PollIntervall = interval;
	}

	_USHORT GetPollInterval()
	{
		return *this->PollIntervall;
	}

	void SetBCDfile(std::string file)
	{
		*this->bcdfile = file;
	}
private:
	_BYTE* NextAction;
	_BYTE* ActionDone;
	_USHORT* PollIntervall;
	_USHORT* RetryCount;
	_ULONG* requestid;
	std::string* bcdfile;
	_IPADDR* referalIP;
	std::string* AdminMessage;
	bool* ServerSelection;
} WDS;

typedef struct DHCP_CLIENT
{
	DHCP_CLIENT()
	{
		this->wds = new WDS();
		this->rbcp = new RBCP();
		this->ipxe = new IPXE();
		this->arch = new DHCP_ARCH(INTEL_X86);
		this->bootfile = new std::string("");
		this->prefix = new std::string("");
		this->vendorid = new DHCP_VENDOR(UNKNOWNNO);
		this->vendorstring = new std::string("PXEClient");
		this->msgtype = new DHCP_MSGTYPE(OFFER);
		this->NextServer = new _IPADDR(0);
		this->isWDSRequest = new bool(false);
		this->isIPXERequest = new bool(false);
		this->isRelayedPacket = new bool(false);
		this->state = new CLIENTSTATE(DHCP_INIT);
	}

	~DHCP_CLIENT()
	{
		delete this->wds;
		delete this->rbcp;
		delete this->ipxe;
		delete this->arch;
		delete this->bootfile;
		delete this->msgtype;
		delete this->NextServer;
		delete this->isWDSRequest;
		delete this->isIPXERequest;
		delete this->isRelayedPacket;
		delete this->state;
		delete this->prefix;
		delete this->vendorid;
		delete this->vendorstring;
	}

	void SetNextServer(_IPADDR ip)
	{
		*this->NextServer = ip;
	}

	_IPADDR GetNextServer()
	{
		return *this->NextServer;
	}

	void Set_State(const CLIENTSTATE& state)
	{
		*this->state = state;
	}

	CLIENTSTATE Get_State()
	{
		return *this->state;
	}

	void Set_Vendor(const DHCP_VENDOR& vendor)
	{
		*this->vendorid = vendor;

		switch(*this->vendorid)
		{
		case PXEClient:
			Set_VendorString("PXEClient");
			break;
		case PXEServer:
			Set_VendorString("PXEServer");
			break;
		case APPLEBSDP:
			Set_VendorString("AAPLBSDPC");
			break;
		}
	}

	DHCP_VENDOR Get_Vendor()
	{
		return *this->vendorid;
	}

	void Set_VendorString(const std::string& vendor)
	{
		*this->vendorstring = vendor;
	}

	std::string Get_VendorString()
	{
		return *this->vendorstring;
	}

	void SetMessageType(DHCP_MSGTYPE type)
	{
		*this->msgtype = type;
	}

	DHCP_MSGTYPE& GetMessageType()
	{
		return *this->msgtype;
	}

	void SetBootfile(std::string file)
	{
		*this->bootfile = file;
	}

	std::string GetPrefix()
	{
		return *this->bootfile;
	}

	void SetPrefix(std::string path)
	{
		*this->prefix = path;
	}

	DHCP_ARCH GetArchitecture()
	{
		return *this->arch;
	}

	void SetArchitecture(DHCP_ARCH arch)
	{
		*this->arch = arch;
	}

	bool GetIsWDSRequest()
	{
		return *this->isWDSRequest;
	}

	bool GetIsIPXERequest()
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

	bool GetIsRelayedPacket()
	{
		return *this->isRelayedPacket;
	}

	void SetIsRelayedPacket(bool is)
	{
		*this->isRelayedPacket = is;
	}

	std::string GetBootfile()
	{
		switch (*this->arch)
		{
		case INTEL_X86:
			*this->prefix = "Boot\\x86\\";
			switch (this->wds->GetNextAction())
			{
			case ABORT:
				*this->bootfile = "Boot\\x86\\abortpxe.com";
				break;
			default:
				*this->bootfile = "Boot\\x86\\pxeboot.n12";
				this->wds->SetBCDfile("Boot\\x86\\default.bcd");
				break;
			}
			break;
		case INTEL_IA32X64:
		case EFI_X86X64:
			*this->prefix = "Boot\\x64\\";
			switch (this->wds->GetNextAction())
			{
			case ABORT:
				*this->bootfile = "Boot\\x64\\abortpxe.com";
				this->wds->SetBCDfile("");
				break;
			default:
				*this->bootfile = "Boot\\x64\\pxeboot.n12";
				this->wds->SetBCDfile("Boot\\x64\\default.bcd");
				break;
			}
			break;
		case EFI_BC:
			*this->prefix = "Boot\\efi\\";
			*this->bootfile = "bootmgfw.efi";
			this->wds->SetBCDfile("default.bcd");
			break;
		default:
			*this->prefix = "Boot\\x86\\";
			*this->bootfile = "Boot\\x86\\wdsnbp.com";
			this->wds->SetBCDfile("");
			break;
		}

		return *this->bootfile;
	}

	WDS* wds = nullptr;
	RBCP* rbcp = nullptr;
	IPXE* ipxe = nullptr;

private:
	bool* isWDSRequest = nullptr;
	bool* isIPXERequest = nullptr;
	bool* isRelayedPacket = nullptr;
	std::string* bootfile = nullptr;
	std::string* prefix = nullptr;
	DHCP_ARCH* arch = nullptr;
	DHCP_MSGTYPE* msgtype = nullptr;
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

	CLIENTSTATE Get_State()
	{
		return *this->tftp_state;
	}

	bool OpenFile(const std::string& path)
	{
		this->filehandle = fopen(path.c_str(), "rb");
		
		fseek(this->filehandle, 0, SEEK_END);

		long bytes = ftell(this->filehandle);
		rewind(this->filehandle);

		this->SetBytesToRead(static_cast<_SIZET>(bytes));

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
		this->block = this->block + 1;
	}

	void SetCurrentBlock(_USHORT block)
	{
		*this->block = block;
	}

	_USHORT GetWindowSize()
	{
		return *this->windowsize;
	}

	void SetWindowSize(_USHORT window)
	{
		*this->windowsize = window;
	}

	_USHORT GetBlockSize()
	{
		return *this->blocksize;
	}

	void SetBlockSize(_USHORT blocksize)
	{
		*this->blocksize = blocksize;
	}

	_USHORT GetMSFTWindow()
	{
		return *this->msftwindow;
	}

	void SetMSFTWindow(_USHORT window)
	{
		*this->msftwindow = window;
	}

	_USHORT GetCurrentBlock()
	{
		return *this->block;
	}

	_SIZET GetBytesToRead()
	{
		return *this->bytesToRead;
	}

	void SetBytesToRead(_SIZET bytes)
	{
		*this->bytesToRead = bytes;
	}

	_SIZET GetBytesRead()
	{
		return *this->bytesread;
	}

	void SetBytesRead(_SIZET bytes)
	{
		if (bytes == 0)
			*this->bytesread = bytes;
		else
			*this->bytesread += bytes;
	}

	std::string GetFilename()
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
		this->windowsize = new _USHORT(1);
		this->filehandle = nullptr;
	}

	~TFTP_CLIENT()
	{
		delete this->filename;
		delete this->block;
		delete this->blocksize;
		delete this->bytesread;
		delete this->bytesToRead;
		delete this->msftwindow;
		delete this->retries;
		delete this->tftp_state;
		delete this->windowsize;
		this->CloseFile();
	}

private:
	_BYTE* retries = nullptr;
	_USHORT* windowsize = nullptr;
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
	NEG_UNICOE	= 0x00000001,
	NEG_OEM		= 0x00000002,
	REQ_TARGET	= 0x00000004,

	NEG_NTLM	= 0x00000200,
	NEG_LOCAL	= 0x00004000,
	NEG_ASIGN	= 0x00008000,

	TAR_DOMAIN	= 0x00010000,
	TAR_SERVER	= 0x00020000,
	TAR_SHARE	= 0x00040000,
	NEG_NTLMV2	= 0x00080000,

	NEG_TARGET	= 0x00800000,
	NEG_128BIT	= 0x20000000,
	NEG_56BIT	= 0x80000000
} NTLMSSP_FLAGS;

typedef enum NTLMSSP_MESSAGETYPE
{
	NTLM_NEGOTIATE		= 0x01000000,
	NTLM_CHALLENGE		= 0x02000000,
	NTLM_AUTHENTICATE	= 0x03000000
} NTLMSSP_MESSAGETYPE;

typedef enum NTLMSSP_TARGETINFO_TYPE
{
	END			= 0x0000,
	NBServer	= 0x0100,
	NBDomain	= 0x0200,
	DNSHost		= 0x0300,
	DNSDomain	= 0x0400,
	TopDNSDom	= 0x0500
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
		delete[] Data;
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

	void Set_Length(const _SIZET length)
	{
		this->Length = static_cast<_USHORT>(length);
		this->AllocatedSpace = this->Length;
	}

	_USHORT Get_Length()
	{
		return this->Length;
	}

	void Set_Position(const _SIZET position)
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
