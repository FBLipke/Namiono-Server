/*
 * Namiono.cpp
 *
 *  Created on: 18.02.2019
 *      Author: lipkegu
 */

#include <Namiono/Namiono.h>
namespace Namiono
{
	
	void Bootstrap()
	{
		printf("[I] Starting...\n");

		using namespace Namiono::Environemnt::Filesystem;

		std::string TFTPRootDir = Combine(CurrentDirectory(), "TFTP_Root");

		printf("[I] Current Directory is: %s\n", CurrentDirectory().c_str());

		if (!IsDirExist(TFTPRootDir))
			if (!MakePath(TFTPRootDir))
			{
				printf("[E] Failed to create Path: %s\n", TFTPRootDir.c_str());

				return;
			}

		printf("[I] TFTP-Root Directory is: %s\n", TFTPRootDir.c_str());

		MakePath(Combine(TFTPRootDir, "Boot"));
		MakePath(Combine(TFTPRootDir, "OSChooser"));

		if (!FileExist(Combine(TFTPRootDir, std::string("Boot\\x86\\wdsnbp.com"))))
			printf("[E] File not found: %s\n(Put this file in the expected Directory).\n",
				Combine(TFTPRootDir, std::string("Boot\\x86\\wdsnbp.com")).c_str());

		if (!FileExist(Combine(TFTPRootDir, std::string("Boot\\x64\\wdsnbp.com"))))
			printf("[E] File not found: %s\n(Put this file in the expected Directory).\n",
				Combine(TFTPRootDir, std::string("Boot\\x64\\wdsnbp.com")).c_str());

		if (!FileExist(Combine(TFTPRootDir, std::string("Boot\\x86\\default.bcd"))))
			printf("[E] File not found: %s\n(Put this file in the expected Directory).\n",
				Combine(TFTPRootDir, std::string("Boot\\x86\\default.bcd")).c_str());

		if (!FileExist(Combine(TFTPRootDir, std::string("Boot\\x64\\default.bcd"))))
			printf("[E] File not found: %s\n(Put this file in the expected Directory).\n",
				Combine(TFTPRootDir, std::string("Boot\\x64\\default.bcd")).c_str());


		std::thread __bstrap_network(Namiono::Network::Bootstrap_Network, TFTPRootDir);

		__bstrap_network.join();
	}
}
