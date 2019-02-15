#include <Namiono-Server.h>

std::string Functions::AddressStr(const _IPADDR ip)
{
	in_addr addr;
	ClearBuffer(&addr, sizeof(addr));

	addr.s_addr = ip;
	return std::string(inet_ntoa(addr));
}

std::string Functions::AsString(const _SIZET input)
{
	std::stringstream ss;
	ss << input;

	return ss.str();
}

int ReadFile(const char* Filename, char* dest, size_t Length)
{
	int res = 1;
	FILE *fil = fopen(Filename, "r");

	if ((fil != NULL) && (fread(dest, 1, Length, fil) == 0))
	{
		fclose(fil);
		res = 0;
	}

	return res;
}

bool Functions::FileExist(const char* Filename)
{
	bool res = false;
	FILE *fil = fopen(Filename, "rb");

	res = (fil != NULL);

	if (res)
		fclose(fil);

	return res;
}

std::vector<std::string> Functions::Split(const std::string& str, const std::string& token)
{
	std::string tmp = str;
	std::vector<std::string> result;
	while (tmp.size())
	{
		auto index = tmp.find(token);
		if (index != std::string::npos)
		{
			result.push_back(tmp.substr(0, index));
			tmp = tmp.substr(index + token.size());

			if (tmp.size() == 0)
				result.push_back(tmp);
		}
		else
		{
			result.push_back(tmp);
			tmp = "";
		}
	}

	return result;
}

/*
 *Compare Memory or Strings...
*/
bool Functions::Compare(const char* p1, const char* p2, const _SIZET length)
{
	bool result = memcmp(p1, p2, length) == 0;

	return result;
}

EXPORT std::string Functions::Replace(std::string& str, const std::string& from, const std::string& to)
{
	auto start_pos = str.find(from);

	while (str.find(from) != std::string::npos)
	{
		start_pos = str.find(from);

		if (start_pos != std::string::npos)
			str = str.replace(start_pos, from.length(), to);
	}

	return str;
}

void Functions::ExtractString(const char* buf, const size_t& size, char* out)
{
	strncpy(out, buf, size - 1);
}

_SIZET Functions::Strip(const char* buffer, const _SIZET buflen)
{
	for (auto i = buflen; buflen > 0; i = i - 1)
		if (static_cast<unsigned char>(buffer[i])
			== static_cast<unsigned char>(0xff))
			return i + 1;

	return buflen;
}

bool Functions::__has_endingslash(const std::string& p)
{
	auto x = p.find_last_of(__pathSeperatorChar(), p.size());

	return x == p.size();
}

std::string Functions::Combine(std::string p1, std::string p2)
{
	std::string _base = __replaceSlash(__fixpath(__resolveMacro(p1)));
	std::string _p2 = p2;
	return _base + _p2;
}

std::string Functions::__replaceSlash(const std::string& p)
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

std::string Functions::__resolveMacro(const std::string & p)
{
	std::string _path = p;
	_path = Functions::Replace(_path, "[TFTP]", __getcwd() + "TFTP_Root");

	return _path;
}

std::string Functions::__fixpath(const std::string& p)
{
	std::string _path = p;

	if (!__has_endingslash(_path))
		_path = _path + __pathSeperatorChar();

	return __replaceSlash(_path);
}

std::string Functions::__getcwd()
{
	char cCurrentPath[255];
	ClearBuffer(cCurrentPath, sizeof cCurrentPath);
	_GET_CUR_WORKINGDIR(cCurrentPath, sizeof cCurrentPath);

	std::string _path = std::string(cCurrentPath);

	if (!__has_endingslash(_path))
		_path = _path + __pathSeperatorChar();

	return _path;
}

std::string Functions::__pathSeperatorChar()
{
	std::string slash = "/";
#ifdef _WIN32
	slash = "\\";
#else
	slash = "/";
#endif // _WIN32

	return slash;
}
