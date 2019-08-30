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
		std::string __replaceSlash(const std::string& p);
		std::string __pathSeperatorChar();
		bool __has_endingslash(const std::string& p);
		std::string CurrentDirectory();
		bool IsDirExist(const std::string& path);
		bool MakePath(const std::string& path);
		std::string Combine(const std::string& p1, const std::string& p2);
		_SIZET FileLength(const std::string& file);
		bool FileExist(const std::string& filename);
		_SIZET FileRead(char* dst, _SIZET length, FILE* handle);
		_SIZET FileWrite(const std::string& filename, const char* src, _SIZET length);
		bool WriteLeaseEntry(const std::string& filename, const std::string& ipaddress, const std::string& mac);
	}
}
