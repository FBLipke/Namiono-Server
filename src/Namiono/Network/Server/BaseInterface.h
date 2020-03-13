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

#pragma once
#include <Namiono/Namiono.h>

namespace Namiono
{
	namespace Network
	{
		class BaseInterface
		{
		public:
			BaseInterface();
			~BaseInterface();

			bool Init();
			bool Start();

			void Listen();
			bool Stop();
			bool Close();
			bool Send();

			void Set_Gateway(const _IPADDR& gateway);
			_IPADDR Get_Gateway() const;

			void Set_Address(const _IPADDR& address);
			_IPADDR Get_Address() const;

			void Set_Port(const _USHORT& port);
			_USHORT Get_Port() const;

		private:
			_IPADDR gateway;
			_IPADDR address;
			_USHORT port;
		};
	}
}
