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

#ifndef ENVIRONMENT_ENVIRONMENT_LINUX_H_
#define ENVIRONMENT_ENVIRONMENT_LINUX_H_
#ifdef __GNUC__
#include <sys/socket.h>
#include <netinet/in.h>
#include  <net/if.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <strings.h>

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>
#include <syslog.h>

typedef _UINT _SIZET;
typedef _UINT _IPADDR;
typedef _UINT _SOCKLENT;
typedef _INT32 _SOCKET;
#define SOCKET_ERROR -1

#define _GET_CUR_WORKINGDIR(p,s) getcwd(p, s);
#define ClearBuffer(x, y) bzero(x, y)
#define _STAT stat
#define _close(s) close(s)
#define _GetLastError strerror(errno)
#define _select(n,r,w,e,t) pselect(n, r, w, e, t, nullptr);
#endif /* ENVIRONMENT_ENVIRONMENT_LINUX_H_ */
#endif