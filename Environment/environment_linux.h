/*
 * environment_windows.h
 *
 *  Created on: 15.02.2019
 *      Author: lipkegu
 */

#ifndef ENVIRONMENT_ENVIRONMENT_LINUX_H_
#define ENVIRONMENT_ENVIRONMENT_LINUX_H_
#ifdef __LINUX__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netdb.h>

typedef _UINT _SIZET;
typedef _UINT _IPADDR;
typedef _UINT _SOCKLENT;
typedef int _SOCKET;
#define SOCKET_ERROR -1

#define _GET_CUR_WORKINGDIR(p,s) getcwd(p, s);
#define ClearBuffer(x, y) bzero(x, y)
#define _STAT stat
#endif /* ENVIRONMENT_ENVIRONMENT_LINUX_H_ */
#endif