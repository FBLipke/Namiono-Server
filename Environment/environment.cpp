/*
 * environment.cpp
 *
 *  Created on: 15.02.2019
 *      Author: lipkegu
 */

#include "environment.h"

void print_Error(const std::string& message, int errorcode)
{
	if (errorcode != 0)
		printf("[E] Error (%d): %s\n", errorcode, message.c_str());
	else
		printf("[E] Error: %s\n", message.c_str());
}

namespace Namiono
{
	namespace Network
	{
#ifdef _WIN32
		bool Init_Winsock(int major, int minor)
		{
			WSADATA wsa;
			ClearBuffer(&wsa, sizeof wsa);

			return WSAStartup(MAKEWORD(major, minor), &wsa) == 0;
		}

		bool Close_Winsock()
		{
			return WSACleanup() == 0;
		}
#endif
		std::string Get_Hostname()
		{
			char hname[64];
			ClearBuffer(hname, sizeof hname);
			int retval = gethostname(hname, sizeof hname);
			if (retval == -1)
			{
				printf("[E] gethostname(): %s\n", strerror(errno));
				return "";
			}

			return std::string(hname);
		}

		std::string AsString(const _SIZET input)
		{
			std::stringstream ss;
			ss << input;

			return ss.str();
		}
	}

	namespace Environemnt
	{
		namespace Filesystem
		{
			std::string __pathSeperatorChar()
			{
				std::string slash = "/";
#ifdef _WIN32
				slash = "\\";
#endif // _WIN32
				return slash;
			}

			_SIZET FileLength(const std::string& file)
			{
				_SIZET bytes = 0;

				FILE* fil = fopen(file.c_str(), "rb");

				if (!fil)
					return -1;

				fseek(fil, 0, SEEK_END);

				bytes = ftell(fil);
				rewind(fil);
				fclose(fil);

				return bytes;
			}

			bool FileExist(const std::string& filename)
			{
				FILE *fil = fopen(filename.c_str(), "rb");

				bool res = (fil != nullptr);

				if (res)
					res = fclose(fil) == 0;

				return res;
			}

			bool __has_endingslash(const std::string& p)
			{
				return p.find_last_of(__pathSeperatorChar(), p.size()) == p.size();
			}

			std::string CurrentDirectory()
			{
				char cCurrentPath[255];
				ClearBuffer(cCurrentPath, sizeof cCurrentPath);

				_GET_CUR_WORKINGDIR(cCurrentPath, sizeof cCurrentPath);

				std::string _path = std::string(cCurrentPath);
				return _path;
			}

			std::string __replaceSlash(const std::string& p)
			{
				std::string _path = p;

				_path = Replace(_path, __pathSeperatorChar() + __pathSeperatorChar(), "/");

				if (_path.find_first_of("/") == 1)
					_path = _path.substr(1);

#ifdef _WIN32
				_path = Replace(_path, "/", __pathSeperatorChar());
#else
				_path = Replace(_path, "\\", __pathSeperatorChar());
#endif // _WIN32
				return _path;
			}

			bool IsDirExist(const std::string& path)
			{
				struct _STAT info;

			    if (_STAT(path.c_str(), &info) != 0)
			        return false;

			    return (info.st_mode & S_IFDIR) != 0;
			}

			std::string Replace(std::string& str, const std::string& from, const std::string& to)
			{
				while (str.find(from) != std::string::npos)
				{
					_SIZET start_pos = str.find(from);

					if (start_pos != std::string::npos)
						str = str.replace(start_pos, from.length(), to);
				}

				return str;
			}

			bool MakePath(const std::string& path)
			{
				int ret = -1;
				_SIZET pos = -1;
#ifdef _WIN32
			    ret = _mkdir(path.c_str());
#else
			    mode_t mode = 0755;
			    ret = mkdir(path.c_str(), mode);
#endif
			    if (ret == 0)
			        return true;

			    switch (errno)
			    {
			    	case ENOENT:
			    		 pos = path.find_last_of(__pathSeperatorChar().c_str());
			    		if (pos == std::string::npos)
			    			return false;

			        	if (!MakePath(path.substr(0, pos)))
			        		return false;
#ifdef _WIN32
			        	return 0 == _mkdir(path.c_str());
#else
			        	return 0 == mkdir(path.c_str(), mode);
#endif
			    	case EEXIST:
			        	return IsDirExist(path);
			    }

	    		return false;
			}

			std::string Combine(const std::string& p1, const std::string& p2)
			{
				std::string _path = p1;

				if (!__has_endingslash(_path))
					_path = _path + __pathSeperatorChar();

				return __replaceSlash(_path + p2);
			}
		}
	}
}
