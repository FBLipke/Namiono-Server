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
