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
	namespace Common
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

		_SIZET FileRead(char* dst, _SIZET length, FILE* handle)
		{
			_SIZET retval = 0;

			retval = fread(dst, sizeof(_BYTE), length, handle);

			if (retval > 0)
				return retval;


			return 0;
		}


		bool WriteLeaseEntry(const std::string& filename, const std::string& ipaddress, const std::string& mac)
		{
			FILE * fp = fopen(filename.c_str(), "wa");

			// 192.168.1.10;00:11:22:33:44:55;

			fprintf(fp, "%s;%s\n", ipaddress.c_str(), mac.c_str());

			fclose(fp);
			return true;
		}

		_SIZET FileWrite(const std::string& filename, const char* src, const _SIZET& length)
		{
			_SIZET retval = 0;

			FILE* fil = fopen(filename.c_str(), "wb");

			if (fil == nullptr)
				return retval;

			retval = fwrite(src, sizeof(_BYTE), length, fil);
			fclose(fil);

			return retval;
		}

		std::string CurrentDirectory()
		{
			char cCurrentPath[MAX_PATH];
			ClearBuffer(cCurrentPath, sizeof cCurrentPath);

			_GET_CUR_WORKINGDIR(cCurrentPath, sizeof cCurrentPath);

			std::string _path = std::string(cCurrentPath);
			return _path;
		}

		std::string __replaceSlash(const std::string& p)
		{
			std::string _path = p;

			_path = Functions::Replace(_path, __pathSeperatorChar() + __pathSeperatorChar(), "/");

			if (_path.find_first_of("/") == 1)
				_path = _path.substr(1);

#ifdef _WIN32
			_path = Functions::Replace(_path, "/", __pathSeperatorChar());
#else
			_path = Functions::Replace(_path, "\\", __pathSeperatorChar());
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

		bool MakePath(const std::string& path)
		{
			_INT32 ret = -1;
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

		std::string Combine(const std::string& p1, const std::string& p2 = "")
		{
			std::string _path = p1;

			if (p1.size() == 0)
				return p2;

			if (!__has_endingslash(_path))
				_path = _path + __pathSeperatorChar();

				return p2.size() != 0 ? __replaceSlash(_path + p2) : __replaceSlash(_path);
		}
	}
}
