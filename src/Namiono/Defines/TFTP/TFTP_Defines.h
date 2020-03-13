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
				ClearBuffer(Value, sizeof(_BYTE));

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
	}
}
