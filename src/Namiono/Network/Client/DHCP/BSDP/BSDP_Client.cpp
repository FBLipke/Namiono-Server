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
		BSDP_Client::BSDP_Client()
		{
			this->MSGTYPE = new BSDP_MSGTYPE(LIST);
			this->Priority = new _USHORT(0x0a);
			this->BootImage = new _UINT(1);
			this->SelectedImage = new _UINT(1);
			this->attributes = new BSDP_ATTIBUTE(Install);
			this->ServerIdent = new _IPADDR(0);
			this->ReplyPort = new _USHORT(68);
		}

		BSDP_Client::~BSDP_Client()
		{
			delete this->attributes;
			this->attributes = nullptr;

			delete this->BootImage;
			this->BootImage = nullptr;

			delete this->Priority;
			this->Priority = nullptr;

			delete this->MSGTYPE;
			this->MSGTYPE = nullptr;

			delete this->ServerIdent;
			this->ServerIdent = nullptr;

			delete this->ReplyPort;
			this->ReplyPort = nullptr;

			delete this->SelectedImage;
			this->SelectedImage = nullptr;
		}

		void BSDP_Client::Set_ServerIdent(const _IPADDR& ident)
		{
			*this->ServerIdent = ident;
		}

		void BSDP_Client::Set_BootImage(const _UINT& id)
		{
			*this->BootImage = id;
		}

		_UINT BSDP_Client::Get_BootImage() const
		{
			return *this->BootImage;
		}

		void BSDP_Client::Set_SelectedImage(const _UINT& id)
		{
			*this->SelectedImage = id;
		}

		_UINT BSDP_Client::Get_SelectedImage() const
		{
			return *this->SelectedImage;
		}

		bool BSDP_Client::Get_ServerIdent() const
		{
			return *this->ServerIdent;
		}

		void BSDP_Client::Set_ReplyPort(const _USHORT& port)
		{
			*this->ReplyPort = port;
		}

		_USHORT BSDP_Client::Get_ReplyPort() const
		{
			return *this->ReplyPort;
		}

		void BSDP_Client::Set_Attributes(const BSDP_ATTIBUTE& attribs)
		{
			*this->attributes = attribs;
		}

		BSDP_ATTIBUTE BSDP_Client::Get_Attributes() const
		{
			return *this->attributes;
		}
	}
}
