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
#include <limits>
#pragma once

typedef unsigned short _USHORT;
typedef unsigned long _ULONG;
typedef unsigned int _UINT;
typedef int _INT32;
typedef long _LONG;
typedef unsigned long long _ULONGLONG;
typedef unsigned char _BYTE;

#ifdef __GNUC__
#include "environment_linux.h"
#else
#include "environment_windows.h"
#endif

#ifdef __BYTE_ORDER
#if defined(_BIG_ENDIAN)
#define __BYTE_ORDER __BIG_ENDIAN
#elif defined(_LITTLE_ENDIAN)
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define BS32(x) x
#define BS16(x) x
#elif __BYTE_ORDER == __BIG_ENDIAN
#define BS32(x) (((uint32_t)(x) >> 24) | (((uint32_t)(x) >> 8) & 0xff00) | (((uint32_t)(x) << 8) & 0xff0000) | ((uint32_t)(x) << 24))
#define BS16(x) (((uint16_t)(x) >> 8) | (((uint16_t)(x) & 0xff) << 8))
#endif
#define LE32(x) (((uint32_t)(x) >> 24) | (((uint32_t)(x) >> 8) & 0xff00) | (((uint32_t)(x) << 8) & 0xff0000) | ((uint32_t)(x) << 24))

#ifdef _WIN32
#define LE16(x) x
#else
#define LE16(x) (((uint16_t)(x) >> 8) | (((uint16_t)(x) & 0xff) << 8))
#endif

#endif /* ENVIRONMENT_ENVIRONMENT_H_ */

void print_Error(const std::string& message, _INT32 errorcode = 0);

