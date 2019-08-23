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
#ifndef ENVIRONMENT_ENVIRONMENT_H_
#define ENVIRONMENT_ENVIRONMENT_H_

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <memory>
#include <fstream>
#include <cmath>
#include <functional>
#include <sstream>
#include <time.h>

#pragma once

typedef unsigned short _USHORT;
typedef unsigned long _ULONG;
typedef unsigned int _UINT;
typedef int _INT32;
typedef unsigned long long _ULONGLONG;
typedef unsigned char _BYTE;

#ifdef __GNUC__
#include "environment_linux.h"
#else
#include "environment_windows.h"
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
#else
#define _BYTE_ORDER __LITTLE_ENDIAN
#endif
#endif
#if _BYTE_ORDER == __LITTLE_ENDIAN
#define BS32(x) x
#define BS16(x) x
#elif _BYTE_ORDER == __BIG_ENDIAN
#define BS16(x) ((static_cast<_USHORT>(x) >> 8) | ((static_cast<_USHORT>(x) & 0xff) << 8))
#define BS32(x) ((static_cast<_UINT>(x) >> 24) | ((static_cast<_UINT>(x) >> 8) & 0xff00) | \
				((static_cast<_UINT>(x) << 8) & 0xff0000) | (static_cast<_UINT>(x) << 24))
#endif

#endif /* ENVIRONMENT_ENVIRONMENT_H_ */

void print_Error(const std::string& message, _INT32 errorcode = 0);

namespace Namiono
{
	namespace Network
	{
		std::string Get_Hostname();
	}
}
