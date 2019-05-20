/*
 * environment.h
 *
 *  Created on: 15.02.2019
 *      Author: lipkegu
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
#include <functional>
#include <sstream>

#pragma once
typedef unsigned short _USHORT;
typedef unsigned long _ULONG;
typedef unsigned int _UINT;
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

namespace Namiono
{
	namespace Network
	{
		std::string Get_Hostname();
	}

	namespace Environemnt
	{
		namespace Filesystem
		{
			std::string __pathSeperatorChar();
			bool __has_endingslash(const std::string& p);
			std::string CurrentDirectory();
			bool IsDirExist(const std::string& path);
			bool MakePath(const std::string& path);
			std::string Combine(const std::string& p1, const std::string& p2);
			_SIZET FileLength(const std::string& file);
			bool FileExist(const std::string& filename);
			std::string Replace(std::string& str, const std::string& from, const std::string& to);
			
		}
	}
}
