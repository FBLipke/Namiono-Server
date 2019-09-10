#include <Environment/environment.h>
#pragma once

namespace Namiono
{
	namespace Network
	{
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
	}
}
