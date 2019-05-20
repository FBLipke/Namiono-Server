/*
 * Namiono.h
 *
 *  Created on: 18.02.2019
 *      Author: lipkegu
 */

#ifndef NAMIONO_NAMIONO_H_
#define NAMIONO_NAMIONO_H_


#include <Environment/environment.h>
#include <Environment/Defines.h>
void print_Error(const std::string& message, int errorcode = 0);
#include <Namiono/Functions.h>
namespace Namiono
{
	void Bootstrap();
}

#include <Namiono/Network/Network.h>
#endif
