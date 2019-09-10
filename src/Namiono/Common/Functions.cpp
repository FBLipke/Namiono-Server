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
		std::string Functions::AddressStr(const _IPADDR ip, const int family)
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
			// packet->Get_TFTPOption("blksize").Value;
			return static_cast<_USHORT>(strtoul(input, nullptr, 0));
		}

		std::string Functions::Get_Hostname()
		{
			char hname[64];
			ClearBuffer(hname, sizeof hname);
			gethostname(hname, sizeof hname);

			return std::string(hname);
		}

		std::string Functions::MacAsString(char* macBuffer, _SIZET length)
		{
			char out[32];
			ClearBuffer(out, sizeof out);
			std::string mac = "";

			sprintf(out, "%02X:%02X:%02X:%02X:%02X:%02X",
				(_BYTE)macBuffer[0],
				(_BYTE)macBuffer[1],
				(_BYTE)macBuffer[2],
				(_BYTE)macBuffer[3],
				(_BYTE)macBuffer[4],
				(_BYTE)macBuffer[5]);

			mac = std::string(out);

			return mac;
		}

		_INT32 Functions::RoundToInteger(double value)
		{
			return _INT32(static_cast<_INT32>(round(value + 0.5)));
		}

		bool Functions::FileExist(const char* Filename)
		{
			FILE *fil = fopen(Filename, "rb");

			bool res = (fil != nullptr);

			if (res)
				fclose(fil);

			return res;
		}

		std::vector<std::string> Functions::Split(const std::string& str, const std::string& token)
		{
			std::string tmp = str;
			std::vector<std::string> result;
			while (tmp.size())
			{
				_SIZET index = tmp.find(token);
				if (index != std::string::npos)
				{
					result.push_back(tmp.substr(0, index));
					tmp = tmp.substr(index + token.size());

					if (tmp.size() == 0)
						result.push_back(tmp);
				}
				else
				{
					result.push_back(tmp);
					tmp = "";
				}
			}

			return result;
		}

		/*
		 * Compare Memory or Strings...
		*/
		bool Functions::Compare(const char* p1, const char* p2, const _SIZET length)
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

		bool Functions::CompareIPAddress(const _IPADDR& ip1, const _IPADDR& ip2, const _SIZET length)
		{
			return memcmp(&ip1, &ip2, length) == 0;
		}


		std::string Functions::AsString(const _SIZET input)
		{
			std::stringstream ss;
			ss << input;

			return ss.str();
		}

		void Functions::ExtractString(const char* buf, const size_t& size, char* out)
		{
			strncpy(out, buf, size - 1);
		}

		_SIZET Functions::Strip(const char* buffer, const _SIZET buflen)
		{
			for (_SIZET i = buflen; buflen > 0; i = i - 1)
				if (static_cast<_BYTE>(buffer[i])
					== static_cast<_BYTE>(0xff))
					return i + 1;

			return buflen;
		}
	}
}
