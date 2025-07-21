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
	_Namiono::_Namiono(_INT32 argc, char* argv[])
	{
		printf("[I] Current Directory is: %s\n", CurrentDirectory().c_str());
		this->settings = std::make_shared<SETTINGS>();

		if (argc > 1)
			for (_INT32 i = 0; i < argc; i++)
			{
				if (memcmp(argv[i], "--rootdir", strlen("--rootdir")) == 0) /* tftp root Directory */
				{
					Get_Settings()->ROOTDIR = std::string(argv[(i + 1)]);
					printf("[D] ARG --rootdir : %s\n", Get_Settings()->ROOTDIR.c_str());
				}

				if (memcmp(argv[i], "--confdir", strlen("--confdir")) == 0) /* tftp config Directory */
				{
					Get_Settings()->CONFDIR = std::string(argv[(i + 1)]);
					printf("[D] ARG --confdir : %s\n", Get_Settings()->CONFDIR.c_str());
				}

				if (memcmp(argv[i], "--srv", strlen("--srv")) == 0) /* Upstream Server */
				{
					Get_Settings()->UPSTREAMSERVER = inet_addr(argv[(i + 1)]);
					printf("[D] ARG --srv : %s\n", Functions::AddressStr(Get_Settings()->UPSTREAMSERVER).c_str());
				}

				if (memcmp(argv[i], "--nbdom", strlen("--nbdom")) == 0)
				{
					Get_Settings()->NBDOMAIN = std::string(argv[(i + 1)]);
					printf("[D] ARG --srv : %s\n", Get_Settings()->NBDOMAIN.c_str());
				}
			}
	}

	_Namiono::~_Namiono()
	{
	}

	bool _Namiono::Init()
	{
		printf("[I] Initializing...\n");

		this->TFTPRootDir = Get_Settings()->ROOTDIR.size() == 0 ?
			Combine(CurrentDirectory(), "TFTP_Root") : Combine(Get_Settings()->ROOTDIR, "");

		if (!IsDirExist(this->TFTPRootDir))
			if (!MakePath(this->TFTPRootDir))
			{
				printf("[E] Failed to create Path: %s\n", this->TFTPRootDir.c_str());

				return false;
			}

		printf("[I] TFTP-Root Directory is: %s\n", this->TFTPRootDir.c_str());

		MakePath(Combine(this->TFTPRootDir, "Boot"));
		MakePath(Combine(this->TFTPRootDir, Combine("Boot","x64")));
		MakePath(Combine(this->TFTPRootDir, Combine("Boot","x86")));
		MakePath(Combine(this->TFTPRootDir, "Config"));

		if (Get_Settings()->CONFDIR.size() == 0)
			Get_Settings()->CONFDIR = Combine(this->TFTPRootDir, "Config");
		else
			MakePath(Get_Settings()->CONFDIR);

		MakePath(Combine(this->TFTPRootDir, "tmp"));
		MakePath(Combine(this->TFTPRootDir, "pxelinux"));
		MakePath(Combine(this->TFTPRootDir, "OSChooser"));

		if (!FileExist(Combine(this->TFTPRootDir, std::string("Boot\\x86\\wdsnbp.com"))))
			printf("[W] File not found: %s\n(Put this file in the expected Directory).\n",
				Combine(this->TFTPRootDir, std::string("Boot\\x86\\wdsnbp.com")).c_str());

		if (!FileExist(Combine(this->TFTPRootDir, std::string("Boot\\x64\\wdsnbp.com"))))
			printf("[W] File not found: %s\n(Put this file in the expected Directory).\n",
				Combine(this->TFTPRootDir, std::string("Boot\\x64\\wdsnbp.com")).c_str());

		if (!FileExist(Combine(this->TFTPRootDir, std::string("Boot\\x86\\default.bcd"))))
			printf("[W] File not found: %s\n(Put this file in the expected Directory).\n",
				Combine(this->TFTPRootDir, std::string("Boot\\x86\\default.bcd")).c_str());

		if (!FileExist(Combine(this->TFTPRootDir, std::string("Boot\\x64\\default.bcd"))))
			printf("[W] File not found: %s\n(Put this file in the expected Directory).\n",
				Combine(this->TFTPRootDir, std::string("Boot\\x64\\default.bcd")).c_str());


		this->network = std::make_shared<Namiono::Network::Network>(Get_Settings(), this->TFTPRootDir);
		this->network.get()->Init();

		return true;
	}

	void _Namiono::Start()
	{
		printf("[I] Starting...\n");
		this->network.get()->Start();
	}

	void _Namiono::Process()
	{
		this->network.get()->Listen();
	}

	void _Namiono::Heartbeat()
	{
		this->network.get()->HeartBeat();
	}

	void _Namiono::Close()
	{
		this->network.get()->Close();
	}

	SETTINGS* _Namiono::Get_Settings()
	{
		return this->settings.get();
	}
}
