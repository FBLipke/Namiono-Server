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

#ifndef NAMIONO_NETWORK_CLIENT_CLIENT_H_
#define NAMIONO_NETWORK_CLIENT_CLIENT_H_

#include <Namiono/Namiono.h>
namespace Namiono
{
	namespace Network
	{
		class Client
		{
		public:
			Client(const ServiceType& sType, const std::string& serverid, const sockaddr_in& hint, const std::string& id)
			{
				this->type = sType;
				this->_hint = hint;
				this->_socketID = serverid;
				this->Set_Client_Hint(this->_hint);
				this->response = nullptr;
				this->_id = id;

				switch (this->type)
				{
				case DHCP_SERVER:
				case BINL_SERVER:
					this->dhcp = new DHCP_CLIENT(this->_socketID);
					if (this->dhcp->GetIsBSDPRequest())
					{
						if (!this->dhcp->GetIsRelayedPacket())
							this->dhcp->bsdp->Set_ReplyPort(this->Get_Port());
					}
					break;
				case TFTP_SERVER:
					this->tftp = new TFTP_CLIENT();
					break;
				default:
					break;
				}
			}

			void Set_Client_Hint(const _IPADDR& addr, const _USHORT port)
			{
				this->_hint = _hint;
				this->_hint.sin_addr.s_addr = addr;
				this->_hint.sin_port = htons(port);
			}

			void Set_Client_Hint(const sockaddr_in& hint)
			{
				this->_hint = hint;

				if (this->_hint.sin_addr.s_addr == 0 && SETTINGS.SOCKET_BROADCAST == 1)
					this->_hint.sin_addr.s_addr = INADDR_BROADCAST;
			}

			void Set_Relay_Hint(const sockaddr_in& hint)
			{
				this->_relay = hint;

				if (this->_relay.sin_addr.s_addr == 0 && SETTINGS.SOCKET_BROADCAST == 1)
					this->_relay.sin_addr.s_addr = INADDR_BROADCAST;
			}

			void Set_Relay_Hint(const _IPADDR& addr, const _USHORT port)
			{
				this->_relay = _hint;
				this->_relay.sin_addr.s_addr = addr;
				this->_relay.sin_port = htons(port);
			}

			void Set_Port(const _USHORT& port)
			{
				this->_hint.sin_port = port;
			}

			_USHORT Get_Port() const
			{
				return this->_hint.sin_port;
			}

			ServiceType Get_ServiceType() const
			{
				return this->type;
			}

			sockaddr_in& Get_Client_Hint()
			{
				return this->_hint;
			}

			sockaddr_in& Get_Relay_Hint()
			{
				return this->_relay;
			}

			const std::string& Get_ID() const
			{
				return this->_id;
			}

			virtual ~Client()
			{
				delete this->tftp;
				this->tftp = nullptr;

				delete this->dhcp;
				this->dhcp = nullptr;

				delete this->response;
				this->response = nullptr;
			}

			TFTP_CLIENT* tftp = nullptr;
			DHCP_CLIENT* dhcp = nullptr;
			Packet* response = nullptr;
			std::string _socketID = "";
		private:
			sockaddr_in _hint;
			sockaddr_in _relay;

			std::string _id = "";

			ServiceType type;
		};
	}
}
#endif /* NAMIONO_NETWORK_CLIENT_CLIENT_H_ */
