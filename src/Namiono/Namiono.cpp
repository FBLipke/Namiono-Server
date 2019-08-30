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

#include <Namiono/Namiono.h>
namespace Namiono
{
	_Namiono::_Namiono()
	{
		printf("[I] Current Directory is: %s\n", CurrentDirectory().c_str());
	}

	_Namiono::~_Namiono()
	{
		delete this->network;
		this->network = nullptr;
	}

	bool _Namiono::Init()
	{
		printf("[I] Initializing...\n");
		this->TFTPRootDir = Combine(CurrentDirectory(), "TFTP_Root");


		if (!IsDirExist(TFTPRootDir))
			if (!MakePath(TFTPRootDir))
			{
				printf("[E] Failed to create Path: %s\n", TFTPRootDir.c_str());

				return false;
			}

		printf("[I] TFTP-Root Directory is: %s\n", TFTPRootDir.c_str());

		MakePath(Combine(TFTPRootDir, "Boot"));
		MakePath(Combine(TFTPRootDir, "Boot\\x64"));
		MakePath(Combine(TFTPRootDir, "Boot\\x86"));
		MakePath(Combine(TFTPRootDir, "Config"));
		MakePath(Combine(TFTPRootDir, "tmp"));
		MakePath(Combine(TFTPRootDir, "pxelinux"));
		MakePath(Combine(TFTPRootDir, "OSChooser"));

		if (!FileExist(Combine(TFTPRootDir, std::string("Boot\\x86\\wdsnbp.com"))))
			printf("[W] File not found: %s\n(Put this file in the expected Directory).\n",
				Combine(TFTPRootDir, std::string("Boot\\x86\\wdsnbp.com")).c_str());

		if (!FileExist(Combine(TFTPRootDir, std::string("Boot\\x64\\wdsnbp.com"))))
			printf("[W] File not found: %s\n(Put this file in the expected Directory).\n",
				Combine(TFTPRootDir, std::string("Boot\\x64\\wdsnbp.com")).c_str());

		if (!FileExist(Combine(TFTPRootDir, std::string("Boot\\x86\\default.bcd"))))
			printf("[W] File not found: %s\n(Put this file in the expected Directory).\n",
				Combine(TFTPRootDir, std::string("Boot\\x86\\default.bcd")).c_str());

		if (!FileExist(Combine(TFTPRootDir, std::string("Boot\\x64\\default.bcd"))))
			printf("[W] File not found: %s\n(Put this file in the expected Directory).\n",
				Combine(TFTPRootDir, std::string("Boot\\x64\\default.bcd")).c_str());


		this->network = new Namiono::Network::Network(TFTPRootDir);
		this->network->Init();

		return true;
	}

	void _Namiono::Start()
	{
		printf("[I] Starting...\n");
		this->network->Start();
	}

	void _Namiono::Process()
	{
		this->network->Listen();
	}

	void _Namiono::Heartbeat()
	{
	}

	void _Namiono::Close()
	{
		printf("[I] Closing...\n");
		this->network->Close();
	}
}
