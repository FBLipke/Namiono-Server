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
#pragma once

namespace Namiono
{
	namespace Network
	{
		class BSDP_Client
		{
		public:
			BSDP_Client();
			~BSDP_Client();
			void Set_ServerIdent(const _IPADDR & ident);
			void Set_BootImage(const _UINT & id);
			_UINT Get_BootImage() const;
			void Set_SelectedImage(const _UINT & id);
			_UINT Get_SelectedImage() const;
			bool Get_ServerIdent() const;
			void Set_ReplyPort(const _USHORT & port);
			_USHORT Get_ReplyPort() const;
			void Set_Attributes(const BSDP_ATTIBUTE & attribs);
			BSDP_ATTIBUTE Get_Attributes() const;
		private:
			BSDP_MSGTYPE* MSGTYPE = nullptr;
			BSDP_VERSION* Version = nullptr;
			BSDP_ATTIBUTE* attributes = nullptr;
			_IPADDR* ServerIdent = nullptr;
			_USHORT* Priority = nullptr;
			_UINT* BootImage = nullptr;
			_UINT* SelectedImage = nullptr;
			_USHORT* ReplyPort = nullptr;
		};
	}
}
