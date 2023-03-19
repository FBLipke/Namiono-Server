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

#include <Namiono/Namiono.h>
namespace Namiono
{
	namespace Common
	{
		std::string Functions::AddressStr(const _IPADDR& ip, const _INT32& family)
		{
			in_addr addr;
			ClearBuffer(&addr, sizeof addr);
			addr.s_addr = ip;

			char _addr[128];
			ClearBuffer(_addr, sizeof _addr);

			inet_ntop(family, &addr, _addr, sizeof _addr);

			return std::string(_addr);
		}

		_USHORT Functions::AsUSHORT(const char* input)
		{
			return static_cast<_USHORT>(strtoul(input, nullptr, 0));
		}

		std::string Functions::Get_Hostname()
		{
			char hname[64];
			ClearBuffer(hname, sizeof hname);
			gethostname(hname, sizeof hname);

			return std::string(hname);
		}

		std::string Functions::MacAsString(char* macBuffer)
		{
			char out[32];
			ClearBuffer(out, sizeof out);
			std::string mac = std::string("");

			sprintf(out, "%02X:%02X:%02X:%02X:%02X:%02X",
				static_cast<_BYTE>(macBuffer[0]),
				static_cast<_BYTE>(macBuffer[1]),
				static_cast<_BYTE>(macBuffer[2]),
				static_cast<_BYTE>(macBuffer[3]),
				static_cast<_BYTE>(macBuffer[4]),
				static_cast<_BYTE>(macBuffer[5]));

			mac = std::string(out);

			return mac;
		}

		_INT32 Functions::RoundToInteger(double value)
		{
			return static_cast<_INT32>(round(value + 0.5));
		}

		std::vector<std::string> Functions::Split(const std::string& str, const std::string& token)
		{
			std::vector<std::string> output;
			std::string::size_type prev_pos = 0, pos = 0;

			while ((pos = str.find(token, pos)) != std::string::npos)
			{
				std::string substring(str.substr(prev_pos, pos - prev_pos));
				output.push_back(substring);
				prev_pos = ++pos;
			}

			output.push_back(str.substr(prev_pos, pos - prev_pos)); // Last word

			return output;
		}

		bool Functions::Compare(const char* p1, const char* p2, const _SIZET& length)
		{
			return memcmp(p1, p2, length) == 0;
		}

		std::string Functions::Replace(std::string& str, const std::string& from, const std::string& to)
		{
			_SIZET start_pos = str.find(from);

			while (str.find(from) != std::string::npos)
			{
				start_pos = str.find(from);

				if (start_pos != std::string::npos)
					str = str.replace(start_pos, from.length(), to);
			}

			return str;
		}

		bool Functions::CompareIPAddress(const _IPADDR& ip1, const _IPADDR& ip2, const _SIZET& length)
		{
			return memcmp(&ip1, &ip2, length) == 0;
		}


		std::string Functions::AsString(const _SIZET& input)
		{
			std::stringstream ss;
			ss << input;

			return ss.str();
		}

		void Functions::ExtractString(const char* buf, const _SIZET& size, char* out)
		{
			strncpy(out, buf, size - 1);
		}

		_SIZET Functions::Strip(const char* buffer, const _SIZET buflen)
		{
			for (_SIZET i = buflen; buflen > 0; i--)
				if (static_cast<_BYTE>(buffer[i]) == static_cast<_BYTE>(0xff))
					return i + 1;

			return buflen;
		}
	}
}
