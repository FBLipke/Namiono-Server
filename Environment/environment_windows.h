/*
 * environment_windows.h
 *
 *  Created on: 15.02.2019
 *      Author: lipkegu
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
#define _GET_CUR_WORKINGDIR(p,s) _getcwd(p, s)
#define _close(s) closesocket(s)
#define ClearBuffer(x, y) memset(x, 0, y)
#define _STAT _stat
namespace Namiono
{
	namespace Network
	{
		bool Init_Winsock(int major, int minor);
		bool Close_Winsock();
	}
}

#endif /* ENVIRONMENT_ENVIRONMENT_WINDOWS_H_ */
