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
		RBCP_Client::RBCP_Client()
		{
			this->item = new _USHORT(0);
			this->layer = new RBCP_LAYER(RBCP_LAYER::Bootfile);

			this->mcastip = new _IPADDR(0);
			this->control = new RBCP_DISCOVERYCONTROL(DISABLE_MCAST);
		}

		void RBCP_Client::Set_Item(const _USHORT& item)
		{
			*this->item = item;
		}

		const _USHORT& RBCP_Client::Get_Item() const
		{
			return *this->item;
		}

		void RBCP_Client::Set_Layer(const RBCP_LAYER& layer)
		{
			*this->layer = layer;
		}

		const RBCP_LAYER& RBCP_Client::Get_Layer() const
		{
			return *this->layer;
		}

		void RBCP_Client::Set_MulticastIP(const _IPADDR& ip)
		{
			*this->mcastip = ip;
		}

		const RBCP_DISCOVERYCONTROL& RBCP_Client::Get_Control()
		{
			return *this->control;
		}

		const _IPADDR& RBCP_Client::Get_MulticastIP() const
		{
			return *this->mcastip;
		}

		RBCP_Client::~RBCP_Client()
		{
			delete this->item;
			this->item = nullptr;

			delete this->layer;
			this->layer = nullptr;

			delete this->control;
			this->control = nullptr;

			delete this->mcastip;
			this->mcastip = nullptr;
		}
	}
}
