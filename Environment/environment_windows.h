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
#ifndef ENVIRONMENT_ENVIRONMENT_WINDOWS_H_
#define ENVIRONMENT_ENVIRONMENT_WINDOWS_H_
#include <winsock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <direct.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "IPHLPAPI.lib")
#ifdef _WIN64
typedef _ULONGLONG _SIZET;
typedef _ULONG _IPADDR;
typedef _SIZET _SOCKET;
typedef _ULONG _SOCKLENT;
#else
#ifdef _WIN32
typedef _UINT _SIZET;
typedef _UINT _IPADDR;
typedef _UINT _SOCKET;
typedef _UINT _SOCKLENT;
#endif
#endif
#define SO_REUSEPORT SO_REUSE_UNICASTPORT
#define _GET_CUR_WORKINGDIR(p,s) _getcwd(p, s)
#define _close(s) closesocket(s)
#define ClearBuffer(x, y) memset(x, 0, y)
#define _GetLastError strerror(errno)
#define _select(n,r,w,e,t) select(0, r, w, e, t)
#define _STAT _stat

#endif /* ENVIRONMENT_ENVIRONMENT_WINDOWS_H_ */
