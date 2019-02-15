#pragma once
#include <vector>
#ifdef _WIN64
typedef unsigned long _SIZET;
typedef unsigned long _IPADDR;
typedef unsigned long _SOCKET;
#define _WINDOWS
#else
#ifdef _WIN32
typedef unsigned int _SIZET;
typedef unsigned int _IPADDR;
typedef unsigned int _SOCKET;
#define _WINDOWS
#endif
#endif

#ifdef _WINDOWS
#include <direct.h>
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#define INLINE __inline
#define ClearBuffer(x, y) memset(x, 0, y);
#define _GET_CUR_WORKINGDIR(p,s) _getcwd(p, s);
#else
#ifdef __GNUC__
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <ifaddrs.h>

typedef unsigned long _SIZET;
typedef unsigned long _IPADDR;
typedef int _SOCKET;
#define _GET_CUR_WORKINGDIR(p,s) getcwd(p, s);
#define INLINE inline
#define EXPORT __attribute__((visibility("default")))
#define SOCKET_ERROR -1
#define closesocket(x) close(x)
#define ClearBuffer(x, y) memset(x, 0, y);
#endif
#endif

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN						1234
#endif

#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN						4321
#endif

#ifndef _BYTE_ORDER
#if defined(__BIG_ENDIAN)
#define _BYTE_ORDER __BIG_ENDIAN
#elif defined(__LITTLE_ENDIAN)
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif
#endif

#if _BYTE_ORDER == __LITTLE_ENDIAN
#define BS32(x) x
#define BS16(x) x
#elif _BYTE_ORDER == __BIG_ENDIAN
#define BS16(x) (((unsigned short)(x) >> 8) | (((unsigned short)(x) & 0xff) << 8))
#define BS32(x) (((unsigned int)(x) >> 24) | (((unsigned int)(x) >> 8) & 0xff00) | \
				(((unsigned int)(x) << 8) & 0xff0000) | ((unsigned int)(x) << 24))
#endif

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

typedef struct TFTP_Option
{
	std::string Option = "";
	_SIZET Length = 0;
	char Value[256];

	TFTP_Option()
	{
		memset(this, 0, sizeof(this));
	}

	TFTP_Option(const std::string& opt, const std::string& value)
	{
		ClearBuffer(Value, (value.size() + 1));

		Option = opt;
		Length = static_cast<_SIZET>(value.size());
		memcpy(&Value, value.c_str(), value.size());
	}

	TFTP_Option(const unsigned char opt)
	{
		ClearBuffer(Value, 1);

		Option = opt;
		Length = 0;
	}

	TFTP_Option(const std::string& opt, _SIZET length, const unsigned short value)
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
	EXPORT DHCP_Option()
	{
		memset(this, 0, sizeof(this));
	};

	EXPORT DHCP_Option(const unsigned char opt, const unsigned char length, const void* value)
	{
		Option = opt;
		Length = length;
		memset(Value, 0, 1024);

		if (length != 0)
			memcpy(&Value, value, Length);
	}

	EXPORT DHCP_Option(const unsigned char opt, const std::string& value)
	{
		Option = opt;
		Length = static_cast<unsigned char>(value.size());
		memset(&Value, 0, 1024);

		if (Length != 0)
			memcpy(Value, value.c_str(), Length);
	}

	EXPORT DHCP_Option(const unsigned char opt, const unsigned char value)
	{
		Option = opt;
		Length = 1;
		memset(&Value, 0, 1024);

		if (Length != 0)
			memcpy(Value, &value, Length);
	}

	EXPORT DHCP_Option(const unsigned char opt, const unsigned short value)
	{
		Option = opt;
		Length = 2;
		memset(&Value, 0, 1024);

		if (Length != 0)
			memcpy(&Value, &value, Length);
	}

	EXPORT DHCP_Option(const unsigned char opt, const unsigned int value)
	{
		Option = opt;
		Length = 4;
		memset(&Value, 0, 1024);

		if (Length != 0)
			memcpy(&Value, &value, Length);
	}

	EXPORT DHCP_Option(const unsigned char opt, const unsigned long value)
	{
		Option = opt;
		Length = 4;
		memset(&Value, 0, 1024);

		if (Length != 0)
			memcpy(&Value, &value, Length);
	}

	EXPORT DHCP_Option(const unsigned char opt, const std::vector<DHCP_Option>& value)
	{
		Option = opt;
		Length = 0;

		auto offset = 0;

		// Get the entire options length!
		for (const auto & option : value)
			Length += option.Length + 2;

		memset(&Value, 0, Length);

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

	EXPORT DHCP_Option(const unsigned char opt)
	{
		Option = opt;
		Length = 0;
		memset(&Value, 0, 1024);
	}

	unsigned char Option;
	unsigned char Length;

	char Value[1024];
} DHCP_Option;

typedef struct BootServerEntry
{
	std::vector<_IPADDR> Addresses;
	std::string Description;
	std::string Bootfile;

	unsigned short Ident = 0;

	BootServerEntry() {}
	BootServerEntry(const unsigned short id, const std::string& desc, std::vector<_IPADDR>& adresses,
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
	std::string Description;
	unsigned short Ident = 0;
	unsigned char DescLength = 0;
	_IPADDR Address = 0;

	BootMenuEntry(const unsigned short id, const std::string& text,
		const BootServerType type = PXEBootstrapServer)
	{
		Ident = id;
		Type = type;
		Description = text;
		DescLength = static_cast<unsigned char>(Description.size());
	}

	BootMenuEntry(const unsigned short id, const std::string& text, const _IPADDR address,
		const BootServerType type = PXEBootstrapServer)
	{
		Ident = id;
		Type = type;
		Address = address;
		Description = text;
		DescLength = static_cast<unsigned char>(Description.size());
	}

	EXPORT ~BootMenuEntry()
	{

	}

	EXPORT BootMenuEntry()
	{

	}
} BootMenuEntry;

typedef enum DHCP_OPCODE
{
	BOOTREQUEST = 0x01,
	BOOTREPLY = 0x02,
	BINL_REQUEST = 0x81,
	BINL_REPLY = 0x82
} DHCP_OPCODE;

typedef enum BINL_OPCODE
{


} BINL_OPCODE;

typedef enum DHCP_MSGTYPE
{
	DISCOVER = 0x01,
	OFFER = 0x02,
	REQUEST = 0x03,
	ACK = 0x05,
	RELEASE = 0x07,
	INFORM = 0x08
} DHCP_MSGTYPE;

typedef enum DHCP_VENDOR
{
	UNKNOWNNO = 0,
	PXEClient = 1,
	PXEServer = 2,
	APPLEBSDP = 3
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

typedef enum EtherBootOption // iPXE, gPXE
{
	Priority = 0x01,
	KeepSan = 0x08,
	NoPXEDhcp = 0xB0,
	BusId = 0xB1,
	BiosDrive = 0xBD,
	Username = 0xBE,
	Password = 0xBF,
	rUsername = 0xC0,
	rpassword = 0xC1,
	Version = 0xEB
} EtherBootOption;

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
	BOOTP_SERVER = 2,
	TFTP_SERVER = 4,
	DHCP6_SERVER = 5,
	BOOTP6_SERVER = 6,
	TFTP6_SERVER = 7,
	BINL_SERVER = 8
} ServiceType;

typedef struct RBCP
{
	RBCP()
	{
		this->item = new unsigned short(0);
		this->layer = new unsigned short(0);

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

	void Set_Item(const unsigned short& item)
	{
		*this->item = item;
	}

	unsigned int Get_Item()
	{
		return *this->item;
	}

	void Set_Layer(const unsigned short& item)
	{
		*this->layer = item;
	}

	unsigned short Get_Layer()
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
	unsigned short* item;
	unsigned short* layer;

	_IPADDR* mcastip;
	RBCP_DISCOVERYCONTROL* control;
} RBCP;

static struct SETTINGS
{
	bool MULTICAST_SUPPORT = false;
	unsigned char DISCOVERY_MODE = UNICAST_ONLY;
	unsigned char PXEBOOTMENUE = 1;
	unsigned char PXEPROMPTTIMEOUT = 255;

	unsigned char PXE_MTFTP_TIMEOUT = 1;
	unsigned char PXE_MTFTP_DELAY = 10;

	std::string PXEPROMP = "Press [F8] to boot from network...";
	std::string DISCOVERY_ADDR = "224.0.1.2";
	
	unsigned short MTFTP_SPORT = 1759;
	unsigned short MTFTP_CPORT = 1758;

	unsigned int SERVER_UPDATE_DELAY = 60;
	unsigned char MAX_HOPS = 4;
} SETTINGS;

typedef struct WDS
{
	WDS()
	{
		this->NextAction = new unsigned char(WDSNBP_OPTION_NEXTACTION::APPROVAL);
		this->ActionDone = new unsigned char(1);
		this->PollIntervall = new unsigned short(SETTINGS.PXE_MTFTP_DELAY);
		this->RetryCount = new unsigned short(65535);
		this->requestid = new unsigned long(1);
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

	void SetRequestID(unsigned long id)
	{
		*this->requestid = id;
	}

	unsigned long GetRequestID()
	{
		return *this->requestid;
	}

	void SetActionDone(const unsigned char& done)
	{
		*this->ActionDone = done;
	}

	unsigned char& GetActionDone()
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

	unsigned char GetNextAction()
	{
		return *this->NextAction;
	}

	void SetRetryCount(unsigned short action)
	{
		*this->RetryCount = action;
	}

	unsigned short GetRetryCount()
	{
		return *this->RetryCount;
	}

	void SetPollInterval(unsigned short interval)
	{
		*this->PollIntervall = interval;
	}

	unsigned short GetPollInterval()
	{
		return *this->PollIntervall;
	}

	void SetBCDfile(std::string file)
	{
		*this->bcdfile = file;
	}
private:
	unsigned char* NextAction;
	unsigned char* ActionDone;
	unsigned short* PollIntervall;
	unsigned short* RetryCount;
	unsigned long* requestid;
	std::string* bcdfile;
	_IPADDR* referalIP;
	std::string* AdminMessage;
	bool* ServerSelection;
} WDS;

typedef struct DHCP_CLIENT
{
	EXPORT DHCP_CLIENT()
	{
		this->wds = new WDS();
		this->rbcp = new RBCP();
		this->arch = new unsigned short(0);
		this->bootfile = new std::string("");
		this->prefix = new std::string("");
		this->vendorid = new DHCP_VENDOR(UNKNOWNNO);

		this->msgtype = new unsigned char(1);
		this->NextServer = new _IPADDR(0);
		this->isWDSRequest = new bool(false);
		this->isRelayedPacket = new bool(false);
		this->state = new CLIENTSTATE(DHCP_INIT);
	}

	EXPORT ~DHCP_CLIENT()
	{
		delete this->wds;
		delete this->rbcp;
		delete this->arch;
		delete this->bootfile;
		delete this->msgtype;
		delete this->NextServer;
		delete this->isWDSRequest;
		delete this->isRelayedPacket;
		delete this->state;
		delete this->prefix;
		delete this->vendorid;
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
	}

	DHCP_VENDOR Get_Vendor()
	{
		return *this->vendorid;
	}

	void SetMessageType(unsigned char type)
	{
		*this->msgtype = type;
	}

	unsigned char GetMessageType()
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

	unsigned short GetArchitecture()
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
private:
	bool* isWDSRequest;
	bool* isRelayedPacket;
	std::string* bootfile;
	std::string* prefix;
	unsigned short* arch;
	unsigned char* msgtype;
	_IPADDR* NextServer;
	CLIENTSTATE* state;
	DHCP_VENDOR* vendorid;

} DHCP_CLIENT;


typedef struct TFTP_CLIENT
{
	void SetTFTPState(unsigned char state)
	{
		*this->tftp_state = state;
	}

	unsigned char GetTFTPState()
	{
		return *this->tftp_state;
	}

	void SetCurrentBlock()
	{
		this->block = this->block + 1;
	}

	void SetCurrentBlock(unsigned short block)
	{
		*this->block = block;
	}

	unsigned short GetWindowSize()
	{
		return *this->windowsize;
	}

	void SetWindowSize(unsigned short window)
	{
		*this->windowsize = window;
	}

	unsigned short GetBlockSize()
	{
		return *this->blocksize;
	}

	void SetBlockSize(unsigned short blocksize)
	{
		*this->blocksize = blocksize;
	}

	unsigned short GetMSFTWindow()
	{
		return *this->msftwindow;
	}

	void SetMSFTWindow(unsigned short window)
	{
		*this->msftwindow = window;
	}

	unsigned short GetCurrentBlock()
	{
		return *this->block;
	}

	long GetBytesToRead()
	{
		return *this->bytesToRead;
	}

	void SetBytesToRead(long bytes)
	{
		*this->bytesToRead = bytes;
	}

	long GetBytesRead()
	{
		return *this->bytesread;
	}

	void SetBytesRead(long bytes)
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
		this->filename = new std::string();
		this->block = new unsigned short(0);
		this->blocksize = new unsigned short(1456);
		this->bytesread = new long(0);
		this->bytesToRead = new long(0);
		this->msftwindow = new unsigned short(27182);
		this->retries = new unsigned char(0);
		this->tftp_state = new unsigned char(CLIENTSTATE::TFTP_INIT);
		this->windowsize = new unsigned short(1);
	}

	~TFTP_CLIENT() {
		delete this->filename;
		delete this->block;
		delete this->blocksize;
		delete this->bytesread;
		delete this->bytesToRead;
		delete this->msftwindow;
		delete this->retries;
		delete this->tftp_state;
		delete this->windowsize;
	}

private:
	unsigned char* retries;
	unsigned short* windowsize;
	unsigned short* msftwindow;
	unsigned short* block;
	unsigned short* blocksize;
	unsigned char* tftp_state;
	std::string* filename;

	long* bytesread;
	long* bytesToRead;
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
	unsigned short	Length;
	const char* Data = nullptr;
public:
	NTLMSSP_TARGETINFO_ENTRY(const NTLMSSP_TARGETINFO_TYPE& type, const char* data, const _SIZET length)
	{
		ClearBuffer(this, sizeof *this);
		this->Length = static_cast<unsigned short>(length);
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
	unsigned short	Length;
	unsigned short	AllocatedSpace;
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
		this->Length = static_cast<unsigned short>(length);
		this->AllocatedSpace = this->Length;
	}

	unsigned short Get_Length()
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
