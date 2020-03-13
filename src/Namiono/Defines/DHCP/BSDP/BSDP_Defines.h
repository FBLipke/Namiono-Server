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
		typedef enum class BSDP_Options
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
