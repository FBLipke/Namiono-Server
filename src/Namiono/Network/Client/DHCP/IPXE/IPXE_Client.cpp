#include <Namiono/Namiono.h>
namespace Namiono
{
	namespace Network
	{
		IPXE_Client::IPXE_Client()
		{
			this->username = new std::string("");
			this->password = new std::string("");
		}

		IPXE_Client::~IPXE_Client()
		{
			delete this->username;
			this->username = nullptr;

			delete this->password;
			this->password = nullptr;
		}

		void IPXE_Client::Set_Username(const std::string& username)
		{
			*this->username = username;
		}

		void IPXE_Client::Set_Password(const std::string& password)
		{
			*this->password = password;
		}

		const std::string& IPXE_Client::Get_Username() const
		{
			return *this->username;
		}

		const std::string& IPXE_Client::Get_Password() const
		{
			return *this->password;
		}
	}
}
