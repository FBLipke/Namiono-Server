#include <Namiono/Namiono.h>

std::string Functions::AddressStr(const _IPADDR ip, const int family)
{
	in_addr addr;
	ClearBuffer(&addr, sizeof addr);
	addr.s_addr = ip;
	
	char _addr[128];
	ClearBuffer(_addr, sizeof _addr);

	inet_ntop(family, &addr, _addr, sizeof _addr);

	return std::string(_addr);
}

std::string Functions::AsString(const _SIZET input)
{
	std::stringstream ss;
	ss << input;

	return ss.str();
}

bool Functions::FileExist(const char* Filename)
{
	FILE *fil = fopen(Filename, "rb");

	bool res = (fil != nullptr);

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
		_SIZET index = tmp.find(token);
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
 *C ompare Memory or Strings...
*/
bool Functions::Compare(const char* p1, const char* p2, const _SIZET length)
{
	return memcmp(p1, p2, length) == 0;
}

std::string Functions::Replace(std::string& str, const std::string& from, const std::string& to)
{
	_SIZET start_pos = str.find(from);

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
	for (_SIZET i = buflen; buflen > 0; i = i - 1)
		if (static_cast<_BYTE>(buffer[i])
			== static_cast<_BYTE>(0xff))
			return i + 1;

	return buflen;
}
