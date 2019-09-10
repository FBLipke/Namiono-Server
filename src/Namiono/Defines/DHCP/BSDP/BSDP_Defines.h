#include <Environment/environment.h>
#pragma once

namespace Namiono
{
	namespace Network
	{
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
			BSDP_LEGACY = 0x10,
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
	}
}
