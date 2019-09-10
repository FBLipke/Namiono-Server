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
