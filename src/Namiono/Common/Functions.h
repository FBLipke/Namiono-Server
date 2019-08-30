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

#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Common
	{
		class Functions
		{
		public:
			static std::string AddressStr(const _IPADDR ip, const int family = 2);
			static std::string Replace(std::string& str, const std::string& from, const std::string& to);
			static std::vector<std::string> Split(const std::string& str, const std::string& token);
			static bool Compare(const char* p1, const char* p2, const _SIZET length);
			static _SIZET Strip(const char* buffer, const _SIZET buflen);
			static std::string AsString(const _SIZET input);
			static std::string Get_Hostname();
			static _INT32 RoundToInteger(double value);
			static void ExtractString(const char* buf, const size_t& size, char* out);
			static bool FileExist(const char* Filename);
			static _USHORT AsUSHORT(const char* input);
			static std::string MacAsString(char * macBuffer, _SIZET length);
			static bool CompareIPAddress(const _IPADDR& ip1, const _IPADDR& ip2, const _SIZET length);
		};
	}
}
